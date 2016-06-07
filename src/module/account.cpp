#include <node.h>

#include "manager.h"
#include "ethaccounts.h"

// Create package exports
void InitAll(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
	AccountManager::Init(exports);
}

NODE_MODULE(eth_account, InitAll)

