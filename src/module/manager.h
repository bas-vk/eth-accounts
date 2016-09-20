#ifndef MANAGER_H_INCL
#define MANAGER_H_INCL

#include <string>
#include <nan.h>

// AccountManager wraps a go-etheruem accounts.Manager instance. It is
// compatible with geth's keystore. It supports both synchronous as
// asynchronous operations.
//
// var manager = new AccountManager(<keystore path>);
// var manager = AccountManager(<keystore path>);
class AccountManager : public Nan::ObjectWrap {
public:
	static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Init(v8::Local<v8::Object> exports);
	
private:
	AccountManager(int id, std::string dir);
	~AccountManager();
	
	static v8::Persistent<v8::Function> constructor;
	
	// New creates a new AccountManager instance. It supports:
	// new AccountManager(...) and AccountManager(...) from javascript.
	static void New(const v8::FunctionCallbackInfo<v8::Value>& info);
	
	// Create a new account
	// @param[in]	passphrase		password
	static NAN_METHOD(NewAccount);
	
	// Unlock account.
	// @param[in] 	account 		address (hex encoded)
	// @param[in]	passphrase		password
	static NAN_METHOD(Unlock);
	
	// Lock account.
	// @param[in]	account			address (hex encoded)
	static NAN_METHOD(Lock);
	
	// Accounts managed by this manager
	// @param[in]	callback		optional callback with array as argument
	static NAN_METHOD(Accounts);
	
	// Sign data
	// @param[in]	account			address (hex encoded)
	// @param[in]	hash            256 bit hash (hex encoded)
	static NAN_METHOD(Sign);
  
	// identifier for wrapped AccountManager in bridge
	int id_;
	
	// keystore location
	std::string dir_;
};

#endif

