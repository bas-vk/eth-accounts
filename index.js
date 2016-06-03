var accounts = require('./build/Release/eth-account.node')

var manager0  = accounts.AccountManager('/tmp/keydir0');

try {
	/*
	var passwd = 'my password';
	var address = manager0.newAccount(passwd);
	console.log("address:", address);
	
	//var unlocked = manager0.unlock(address, 'invalid' + passwd);
	//console.log("address", address, " -> unlocked:", unlocked);
	
	var unlocked = manager0.unlock(address, passwd);
	console.log("address", address, " -> unlocked:", unlocked);
	
	manager0.newAccount(passwd, function(err, address) {
		if (err) {
			console.log("callback", "|", address, "|");
		} else {
			console.log("callback error", ":", err);
		}
	});
	
	manager0.unlock(address, passwd, function(err, unlocked) {
		if (err) {
			console.log("unlock failed:", err);
		} else {
			console.log("unlock result: ", unlocked);
		}
	});
	
	console.log(manager0.accounts());
	*/
	manager0.accounts(function(err, accounts) {
		console.log("callback", err, accounts);
	});
	
	
} catch (e) {
	console.log("error", e);
}

setTimeout(function() {
    console.log('the end...');
}, 10000);

