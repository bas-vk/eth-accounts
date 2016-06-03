#include <string>

#include "manager.h"
#include "async.h"
#include "bridge.h"

#include <iostream>

AccountManager::AccountManager(int id, std::string dir)
    : id_(id)
    , dir_(dir) {
}

AccountManager::~AccountManager() {
    // remove from bridge
    DeleteAccountManager(id_);
}

v8::Persistent<v8::Function> AccountManager::constructor;

// Init creates the exports for the AccountManager class
void AccountManager::Init(v8::Local<v8::Object> exports) {
    v8::Isolate* isolate(exports->GetIsolate());

    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl(v8::FunctionTemplate::New(isolate, New));
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "AccountManager"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototypes
    SetPrototypeMethod(tpl, "newAccount", NewAccount);
    SetPrototypeMethod(tpl, "unlock", Unlock);
    SetPrototypeMethod(tpl, "lock", Lock);
    SetPrototypeMethod(tpl, "accounts", Accounts);
    SetPrototypeMethod(tpl, "sign", Sign);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(v8::String::NewFromUtf8(isolate, "AccountManager"),
        tpl->GetFunction());
}

void AccountManager::New(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    if (info.IsConstructCall()) {
        // Invoked as constructor: `new AccountManager(...)`
        if (!info[0]->IsString()) {
            isolate->ThrowException(v8::String::NewFromUtf8(isolate, "expect filepath as argument"));
            return;
        }

        std::string keystorePath(*v8::String::Utf8Value(info[0]));

        GoInt id = NewAccountManager((char*)keystorePath.c_str());

        AccountManager* instance = new AccountManager(int(id), keystorePath);
        instance->Wrap(info.This());

        return;
    }
    else {
        // Invoked as plain function `AccountManager(...)`, turn into construct call.
        const int argc = 1;
        v8::Local<v8::Value> argv[argc] = { info[0] };
        v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
        info.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
}

void AccountManager::NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = { args[0] };
    v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();

    args.GetReturnValue().Set(instance);
}

NAN_METHOD(AccountManager::NewAccount) {
    v8::Isolate* isolate = info.GetIsolate();

    if (!info[0]->IsString()) {
        isolate->ThrowException(v8::String::NewFromUtf8(isolate, "expect passphrase as argument"));
        return;
    }

    AccountManager* manager(ObjectWrap::Unwrap<AccountManager>(info.Holder()));

    std::string passphrase(*v8::String::Utf8Value(info[0]));

    if (info[1]->IsFunction()) {
        // user supplied a callback, do work in the background and call
        // callback with results
        Nan::Callback* callback(new Nan::Callback(v8::Local<v8::Function>::Cast(info[1])));
        Nan::AsyncQueueWorker(new ethaccount::CreateAccountWorker(callback, manager->id_, passphrase));
        info.GetReturnValue().SetUndefined();
    }
    else {
        GoString passwd = { passphrase.c_str(), int(passphrase.size()) };

        auto address = (char*)malloc(42 + 1);
        memset(address, 0, 43);

        auto error = (char*)malloc(128 + 1);
        memset(error, 0, 129);

        // call go bridge NewAccount
        bool success = ::NewAccount(manager->id_, passwd, address, error);

        if (success) {
            info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, address));
        }
        else {
            isolate->ThrowException(v8::String::NewFromUtf8(isolate, error));
        }

        free(error);
        free(address);
    }
}

NAN_METHOD(AccountManager::Unlock) {
    v8::Isolate* isolate = info.GetIsolate();

    if (!info[0]->IsString() || !info[1]->IsString()) {
        isolate->ThrowException(v8::String::NewFromUtf8(isolate, "expect address and passphrase as argument"));
        return;
    }

    v8::String::Utf8Value address(info[0]->ToString());
    v8::String::Utf8Value password(info[1]->ToString());

    AccountManager* manager(ObjectWrap::Unwrap<AccountManager>(info.Holder()));

    if (info[2]->IsFunction()) {
        // user supplied a callback, do work in the background and call callback with results
        Nan::Callback* callback(new Nan::Callback(v8::Local<v8::Function>::Cast(info[2])));
        Nan::AsyncQueueWorker(new ethaccount::UnlockAccountWorker(callback, manager->id_, *address, *password));

        info.GetReturnValue().SetUndefined();

        return;
    }

    char error[1025];
    memset(error, 0, sizeof(error));

    GoString addr = { *address, int(address.length()) };
    GoString pass = { *password, int(password.length()) };

    bool unlocked = ::UnlockAccount(manager->id_, addr, pass, error);

    info.GetReturnValue().Set(v8::Boolean::New(isolate, unlocked));
    //if (error[0] != 0) { // got an error, throw it
    //	isolate->ThrowException(v8::String::NewFromUtf8(isolate, error));
    //}
}

NAN_METHOD(AccountManager::Lock) {
    v8::Isolate* isolate = info.GetIsolate();

    if (!info[0]->IsString()) {
        isolate->ThrowException(v8::String::NewFromUtf8(isolate, "expect address as argument"));
        return;
    }

    v8::String::Utf8Value address(info[0]->ToString());

    AccountManager* manager(ObjectWrap::Unwrap<AccountManager>(info.Holder()));

    if (info[1]->IsFunction()) { // async
        // user supplied a callback, do work in the background and call callback with results
        Nan::Callback* callback(new Nan::Callback(v8::Local<v8::Function>::Cast(info[1])));
        Nan::AsyncQueueWorker(new ethaccount::LockAccountWorker(callback, manager->id_, *address));
        info.GetReturnValue().SetUndefined();
        return;
    }

    GoString addr = { *address, int(address.length()) };
    bool locked = ::LockAccount(manager->id_, addr);

    info.GetReturnValue().Set(v8::Boolean::New(isolate, locked));
}

NAN_METHOD(AccountManager::Accounts) {
    v8::Isolate* isolate = info.GetIsolate();
    AccountManager* manager(ObjectWrap::Unwrap<AccountManager>(info.Holder()));

    if (info[0]->IsFunction()) { // async
        Nan::Callback* callback(new Nan::Callback(v8::Local<v8::Function>::Cast(info[0])));
        Nan::AsyncQueueWorker(new ethaccount::ListAccountsWorker(callback, manager->id_));
        info.GetReturnValue().SetUndefined();
    }
    else {
        GoString accounts;
        int nAccounts = ::Accounts(manager->id_, &accounts);

        // chunck accounts back to addresses array
        int addressSize = 42;
        v8::Local<v8::Array> result = Nan::New<v8::Array>(nAccounts);

        for (int i = 0; i < nAccounts; ++i) {
            Nan::Set(result, i, v8::String::NewFromUtf8(isolate, accounts.p + (i * addressSize), v8::String::kNormalString, addressSize));
        }

        info.GetReturnValue().Set(result);
    }
}

NAN_METHOD(AccountManager::Sign) {
    v8::Isolate* isolate = info.GetIsolate();
    AccountManager* manager(ObjectWrap::Unwrap<AccountManager>(info.Holder()));

    v8::String::Utf8Value address(info[0]->ToString());
    v8::String::Utf8Value data(info[1]->ToString());

    if (info[2]->IsFunction()) { // async
        Nan::Callback* callback(new Nan::Callback(v8::Local<v8::Function>::Cast(info[2])));
        Nan::AsyncQueueWorker(new ethaccount::SignDataWorker(callback, manager->id_, *address, *data));
        info.GetReturnValue().SetUndefined();
    }
    else {
        GoString addr = { *address, int(address.length()) };
        GoString d = { *data, int(data.length()) };
        GoString signature;

        char error[1025];
        memset(error, 0, sizeof(error));

        bool success = ::Sign(manager->id_, addr, d, &signature, error);
        if (success) {
            info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, signature.p));
        }
        else {
            isolate->ThrowException(v8::String::NewFromUtf8(isolate, error));
        }
    }
}

