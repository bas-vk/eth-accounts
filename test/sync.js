var assert = require('chai').assert,
    expect = require("chai").expect;
    temp = require('temp'),
    rmdir = require('rimraf'),
    ethUtils = require('ethereumjs-util')
    accounts = require('bindings')('eth-account');

describe('Sync create', function() {
	this.timeout(10000);
	
	var path = temp.path();
	var manager = accounts.newManager(path);
	var passwd = 'my passwd';
	
	after(function() {
		rmdir(path, function(error) {
			if (error) {
				console.log('unable to remove test keystore directory', path, error);
			}
		});
	});
	
	it('new account', function() {
		var accountsBefore = manager.accounts();
		// throws on failure
		var address = manager.newAccount(passwd);
		
		expect(address.length).to.equal(42);
		expect(address.substr(0, 2)).to.equal("0x");	
			
		var accountsAfter = manager.accounts();
				
		// test if the newly created account is added to the list
		expect(accountsBefore.length + 1).to.equal(accountsAfter.length);
		
		// verify that the account list has the newly created account
		assert.notEqual(-1, accountsAfter.indexOf(address));
	});
});
	
describe('Sync unlock', function() {
	this.timeout(10000);
	
	var path = temp.path();
	var manager = accounts.newManager(path);
	var passwd = 'my passwd';

	after(function() {
		rmdir(path, function(error) {
			if (error) {
				console.log('unable to remove test keystore directory', path, error);
			}
		});
	});
		
	it('account with valid password', function() {
		var account = manager.newAccount(passwd);		
		expect(manager.unlock(account, passwd)).to.be.true;
	});
	
		
	it('account with invalid password', function() {
		var account = manager.newAccount(passwd);		
		expect(manager.unlock(account, 'invalid' + passwd)).to.be.false;
	});
	
	it('unknown account', function() {
		var unknownAccount = '0x391694e7e0b0cce554cb130d723a9d27458f9298';
		expect(manager.unlock(unknownAccount, passwd)).to.be.false;
	});
	
	it('invalid account', function() {
		var unknownAccount = '0xab';
		expect(manager.unlock(unknownAccount, passwd)).to.be.false;
	});
});

describe('Sync lock', function() {
	this.timeout(10000);
	
	var path = temp.path();
	var manager = accounts.newManager(path);
	var passwd = 'my passwd';
	
	it('account', function() {
		var address = manager.newAccount(passwd);
		
		// make sure account is locked
		var data = '0x0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef';
		var sign = function () { manager.sign(address, data); };
		expect(sign).to.throw('account is locked');
		
		// unlock and make sure we can sign data
		expect(manager.unlock(address, passwd)).to.be.true;
		var sign = manager.sign(address, data);
		expect(sign.length).to.equal(132);
		
		// verify that account is locked	
		expect(manager.lock(address)).to.be.true;
		var sign = function () { manager.sign(address, data); };
		expect(sign).to.throw('account is locked');		
	});
});

describe('Sync sign', function() {
	this.timeout(10000);
	
	var path = temp.path();
	var manager = accounts.newManager(path);
	var passwd = 'my passwd';
	
	after(function() {
		rmdir(path, function(error) {
			if (error) {
				console.log('unable to remove test keystore directory', path, error);
			}
		});
	});
	
	
	it('data and verify signature', function() {
		var address = manager.newAccount(passwd);
		var data = '0x0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef';
		
		assert(manager.unlock(address, passwd));
		
		var sig = manager.sign(address, data);
		expect(sig.length).to.equal(132);
		
		// strip 0x
		sig = sig.substr(2);
		
		// test signature
		var d = new Buffer(data.substr(2), 'hex');
		var r = new Buffer(sig.substr(0, 64), 'hex');
		var s = new Buffer(sig.substr(64, 64), 'hex');
		var v = parseInt(sig.substr(128, 2), 16) + 27;
		
		var pubKey = ethUtils.ecrecover(d, v, r, s);	
		var r = ethUtils.publicToAddress(pubKey);
		
		// verify signing was done with account associated with address
    	assert.equal('0x' + r.toString('hex'), address);
	});
	
	it('data with locked account', function() {
		var account = manager.newAccount(passwd);
		var data = '0x0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef';
		
		var sign = function () { manager.sign(account, data); };
		expect(sign).to.throw('account is locked');
	});
});

