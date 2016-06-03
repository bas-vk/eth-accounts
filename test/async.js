var assert = require('chai').assert,
    expect = require("chai").expect;
    temp = require('temp'),
    rmdir = require('rimraf'),
    ethUtils = require('ethereumjs-util')
    accounts = require('../build/Release/eth-account.node');

describe('Async create', function() {
	this.timeout(10000);
	
	var path = temp.path();
	var manager = accounts.AccountManager(path);
	var passwd = 'my passwd';
	
	after(function() {
		rmdir(path, function(error) {
			if (error) {
				console.log('unable to remove test keystore directory', path, error);
			}
		});
	});
	
	it('new account', function(done) {
		var accountsBefore = manager.accounts();
		// throws on failure
		manager.newAccount(passwd, function(err, address) {
			expect(err).to.be.null;
			
			var accountsAfter = manager.accounts();				
			
			// test if the newly created account is added to the list
			expect(accountsBefore.length + 1).to.equal(accountsAfter.length);
					
			// verify that the account list has the newly created account
			assert.notEqual(-1, accountsAfter.indexOf(address));
			
			done();
		});		
	});
});

describe('Async unlock', function() {
	this.timeout(30000);
	
	var path = temp.path();
	var manager = accounts.AccountManager(path);
	var passwd = 'my passwd';

	after(function() {
		rmdir(path, function(error) {
			if (error) {
				console.log('unable to remove test keystore directory', path, error);
			}
		});
	});
		
	it('account with valid password', function(done) {
		var account = manager.newAccount(passwd);
		
		manager.unlock(account, passwd, function(err, unlocked) {
			expect(unlocked).to.be.true;
			done();
		});
	});
	
	it('account with invalid password', function(done) {
		var account = manager.newAccount(passwd);
		
		manager.unlock(account, 'invalid' + passwd, function(err, unlocked) {
			expect(unlocked).to.be.false;
			done();
		});		
	});
});

describe('Async lock', function() {
	this.timeout(10000);
	
	var path = temp.path();
	var manager = accounts.AccountManager(path);
	var passwd = 'my passwd';
	
	it('account', function(done) {
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
		manager.lock(address, function(locked) {
			expect(locked).to.be.true;
			var sign = function () { manager.sign(address, data); };
			expect(sign).to.throw('account is locked');
			
			done();
		});		
	});
});

describe('Async sign', function() {
	this.timeout(10000);
	
	var path = temp.path();
	var manager = accounts.AccountManager(path);
	var passwd = 'my passwd';
	
	after(function() {
		rmdir(path, function(error) {
			if (error) {
				console.log('unable to remove test keystore directory', path, error);
			}
		});
	});
	
	it('data and verify signature', function(done) {
		var address = manager.newAccount(passwd);
		var data = '0x0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef';
		
		assert(manager.unlock(address, passwd));
		
		var sig = manager.sign(address, data, function(err, sig) {
			expect(err).to.be.null;
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
		
			assert.equal('0x' + r.toString('hex'), address);
			
			done();
		});		
	});
	
	it('data with locked account', function(done) {
		var account = manager.newAccount(passwd);
		var data = '0x0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef';
		
		manager.sign(account, data, function(err, sig) {
			expect(err).to.equal('account is locked')
			done();
		});	
	});
});
