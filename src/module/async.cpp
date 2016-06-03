#include "async.h"
#include "bridge.h"

#include <cstring>

namespace ethaccount {

inline void SecureWipeBuffer(char* buf, size_t n) {
    volatile char* p = buf;
    asm volatile("rep stosb" : "+c"(n), "+D"(p) : "a"(0) : "memory");
}

CreateAccountWorker::CreateAccountWorker(Nan::Callback* callback, int id, std::string password)
    : Nan::AsyncWorker(callback)
    , id(id)
    , passwd(password)
    , created(false) {

    memset(newAddress, 0, sizeof(newAddress));
    memset(errorMsg, 0, sizeof(errorMsg));
}

CreateAccountWorker::~CreateAccountWorker() {
    SecureWipeBuffer((char*)passwd.c_str(), passwd.size());
}

void CreateAccountWorker::Execute() {
    // call go bridge NewAccount, this takes some due to the scrypt algorithm
    GoString pass = { passwd.c_str(), int(passwd.size()) };
    created = ::NewAccount(id, pass, newAddress, errorMsg);
}

void CreateAccountWorker::HandleOKCallback() {
    // We have the results, and we're back in the event loop.
    v8::Isolate* isolate(v8::Isolate::GetCurrent());
    v8::Local<v8::Value> argv[2];

    if (created) { // account was successful created
        argv[0] = v8::Null(isolate);
        argv[1] = v8::String::NewFromUtf8(isolate, newAddress);
    }
    else {
        argv[0] = v8::String::NewFromUtf8(isolate, errorMsg);
        argv[1] = v8::Null(isolate);
    }

    callback->Call(2, argv);
}

UnlockAccountWorker::UnlockAccountWorker(Nan::Callback* callback, int id, std::string address, std::string password) 
	: Nan::AsyncWorker(callback)
    , id(id)
    , address(address)
    , passwd(password)
    , unlocked(false) {
    memset(errorMsg, 0, sizeof(errorMsg));
}

UnlockAccountWorker::~UnlockAccountWorker() {
    SecureWipeBuffer((char*)passwd.c_str(), passwd.size());
}

void UnlockAccountWorker::Execute() {
    GoString pass = { passwd.c_str(), int(passwd.size()) };
    GoString addr = { address.c_str(), int(address.size()) };

    // call go bridge UnlockAccount, this takes some due to the scrypt algorithm
    unlocked = ::UnlockAccount(id, addr, pass, errorMsg);
}

void UnlockAccountWorker::HandleOKCallback() {
    // We have the results, and we're back in the event loop.
    v8::Isolate* isolate(v8::Isolate::GetCurrent());
    v8::Local<v8::Value> argv[2];

    if (unlocked) {
        argv[0] = v8::Null(isolate);
    }
    else {
        argv[0] = v8::String::NewFromUtf8(isolate, errorMsg);
    }

    argv[1] = v8::Boolean::New(isolate, unlocked);
    callback->Call(2, argv);
}

LockAccountWorker::LockAccountWorker(Nan::Callback* callback, int id, std::string address)
    : Nan::AsyncWorker(callback)
    , id(id)
    , address(address)
    , locked(false) {
}

void LockAccountWorker::Execute() {
    GoString addr = { address.c_str(), int(address.size()) };
    locked = ::LockAccount(id, addr);
}

void LockAccountWorker::HandleOKCallback() {
    // We have the results, and we're back in the event loop.
    v8::Isolate* isolate(v8::Isolate::GetCurrent());
    v8::Local<v8::Value> argv[1] = { v8::Boolean::New(isolate, locked) };

    callback->Call(1, argv);
}

ListAccountsWorker::ListAccountsWorker(Nan::Callback* callback, int id)
    : Nan::AsyncWorker(callback)
    , id(id) {
}

void ListAccountsWorker::Execute() {
    GoString accountsConcat;
    nAccounts = ::Accounts(id, &accountsConcat);
    if (nAccounts > 0) {
        accounts.assign(accountsConcat.p, accountsConcat.n);
    }
}

void ListAccountsWorker::HandleOKCallback() {
    // We have the results, and we're back in the event loop.
    v8::Isolate* isolate(v8::Isolate::GetCurrent());

    // chunck accounts back to addresses array "0x<160 bit hex encoded>"
    static const int addressSize = 42;

    result = Nan::New<v8::Array>(nAccounts);
    for (int i = 0; i < nAccounts; ++i) {
        Nan::Set(result, i, v8::String::NewFromUtf8(isolate, accounts.substr(i * addressSize, 42).c_str(), v8::String::kNormalString, addressSize));
    }

    v8::Local<v8::Value> argv[2] = { v8::Null(isolate), result };
    callback->Call(2, argv);
}

SignDataWorker::SignDataWorker(Nan::Callback* callback, int id, std::string address, std::string data)
    : Nan::AsyncWorker(callback)
    , id(id)
    , address(address)
    , data(data) {
    memset(errorMsg, 0, sizeof(errorMsg));
}

void SignDataWorker::Execute() {
    GoString addr = { address.c_str(), int(address.length()) };
    GoString d = { data.c_str(), int(data.length()) };
    GoString sig;

    success = ::Sign(id, addr, d, &sig, errorMsg);
    if (success) {
        signature = sig.p;
    }
}

void SignDataWorker::HandleOKCallback() {
    // We have the results, and we're back in the event loop.
    v8::Isolate* isolate(v8::Isolate::GetCurrent());
    v8::Local<v8::Value> argv[2];

    if (success) {
        argv[0] = v8::Null(isolate);
        argv[1] = v8::String::NewFromUtf8(isolate, signature.c_str());
    }
    else {
        argv[0] = v8::String::NewFromUtf8(isolate, errorMsg);
        argv[1] = v8::Null(isolate);
    }

    callback->Call(2, argv);
}

}
