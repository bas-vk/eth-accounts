var exec = require('child_process').exec;

if (process.platform == 'darwin') {
	exec('install_name_tool -change libethaccounts @loader_path/libethaccounts.dylib `pwd`/build/Debug/eth-account.node')
	exec('install_name_tool -change libethaccounts @loader_path/libethaccounts.dylib `pwd`/build/Release/eth-account.node')
}