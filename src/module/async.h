#ifndef ASYNC_H_INCL
#define ASYNC_H_INCL

#include <nan.h>

namespace ethaccount {

class CreateAccountWorker : public Nan::AsyncWorker {
public:
    CreateAccountWorker(Nan::Callback* callback, int id, std::string password);
    ~CreateAccountWorker();

    void Execute();

    // We have the results, and we're back in the event loop.
    void HandleOKCallback();

private:
    int id;
    std::string passwd;
    bool created;
    char newAddress[43];
    char errorMsg[1025];
};

class UnlockAccountWorker : public Nan::AsyncWorker {
public:
    UnlockAccountWorker(Nan::Callback* callback, int id, std::string address, std::string password);
    ~UnlockAccountWorker();

    void Execute();

    // We have the results, and we're back in the event loop.
    void HandleOKCallback();

private:
    int id;
    std::string address;
    std::string passwd;
    bool unlocked;
    char errorMsg[1025];
};

class LockAccountWorker : public Nan::AsyncWorker {
public:
    LockAccountWorker(Nan::Callback* callback, int id, std::string address);

    void Execute();

    // We have the results, and we're back in the event loop.
    void HandleOKCallback();

private:
    int id;
    std::string address;
    bool locked;
};

class ListAccountsWorker : public Nan::AsyncWorker {
public:
    ListAccountsWorker(Nan::Callback* callback, int id);

    void Execute();

    // We have the results, and we're back in the event loop.
    void HandleOKCallback();

private:
    int id;
    std::string accounts;
    int nAccounts;
    v8::Local<v8::Array> result;
};

class SignDataWorker : public Nan::AsyncWorker {
public:
    SignDataWorker(Nan::Callback* callback, int id, std::string address, std::string data);

    void Execute();

    // We have the results, and we're back in the event loop.
    void HandleOKCallback();

private:
    int id;
    std::string address;
    std::string data;
    bool success;
    std::string signature;
    char errorMsg[1025];
};
}

#endif

