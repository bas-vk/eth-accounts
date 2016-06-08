# Introduction
Bindings (subset) to the go-ethereum [accounts](https://godoc.org/github.com/ethereum/go-ethereum/accounts) package.

## Requirements

Building this package requires [go >=1.6.2](https://golang.org/dl/) and a c++ compiler (g++, clang++) to be installed.

You will also need to fetch and build [`ethereum/go-ethereum`](https://github.com/ethereum/go-ethereum). Clone the repo into your `$GOPATH`:

```bash
$ cd $GOPATH/src
$ git clone https://github.com/ethereum/go-ethereum github.com/ethereum/go-ethereum
$ cd github.com/ethereum/go-ethereum
$ make
```

And there are a few other libraries you will need:

```bash
$ cd $GOPATH/src
$ git clone https://github.com/pborman/uuid.git github.com/pborman/uuid
$ git clone https://github.com/rjeczalik/notify.git github.com/rjeczalik/notify
$ git clone https://github.com/golang/crypto.git golang.org/x/crypto
```

## Installation

```shell
$ git clone https://github.com/bas-vk/eth-accounts.git
$ cd eth-accounts
$ npm install
$ cd build
$ make
```

The module will be stored in the `build/Release` directory together with a libethaccounts library. On Linux this library needs to be saved in a directory where the loader can find it when node loads the module.

## Install shared library Linux
This requires 2 steps, place the library in a location where the loader will search and order the loader to update its cache. And verify with the ldd utility that the loader is able to find the library.

```shell
$ sudo cp build/libethaccounts.so /usr/local/lib/
$ sudo ldconfig -n /usr/local/lib
$ ldd build/Release/eth-account.node 
	linux-vdso.so.1 (0x00007ffdbbb6d000)
	libethaccounts.so => /usr/local/lib/libethaccounts.so (0x00007fec91bf4000)
	libstdc++.so.6 => /usr/lib/libstdc++.so.6 (0x00007fec9186d000)
	libm.so.6 => /usr/lib/libm.so.6 (0x00007fec91569000)
	libgcc_s.so.1 => /usr/lib/libgcc_s.so.1 (0x00007fec91352000)
	libpthread.so.0 => /usr/lib/libpthread.so.0 (0x00007fec91135000)
	libc.so.6 => /usr/lib/libc.so.6 (0x00007fec90d94000)
	/usr/lib64/ld-linux-x86-64.so.2 (0x0000561d8fa55000)
```

## Unittests
```shell
$ npm test
```


## Usage
```js
var accounts = require('bindings')('eth-account');

// create account manager which stores/reads keys in `keystorePath`
var keystorePath = '/tmp/keystore';
var manager = accounts.newManager(keystorePath);

// create account
var passwd = 'my password';
var address = manager.newAccount(passwd);

// create account asyn
manager.newAccount(passwd, function(err, address) {
    //...
});

// list accounts from keystore
var accounts = manager.accounts();

// list accounts from keystore async
manager.accounts(function(accounts) {
    //...
});

// unlock account
var isUnlocked = manager.unlock(address, passwd);

// or async
manager.unlock(address, passwd, function(err, unlocked) {
    //...
});

// Sign data, data must be a 256 bits hex encoded string, the returned signature is also hex encoded.
// See unittests for an example how to verify the signature.
var data = '0x0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef';
var signature = manager.sign(address, data);

// or async
manager.sign(address, data, function(err, signature) {
	// ...
});

// lock account
var isLocked = manager.lock(address);

// or async
manager.lock(address, function(err, locked) {
	//...
});
```

