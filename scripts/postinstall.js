var exec = require('child_process').exec;

if (process.platform == 'darwin') {
	exec('install_name_tool -change libethaccounts `pwd`/build/libethaccounts.dylib ./build/Debug/eth-account.node')
	exec('install_name_tool -change libethaccounts `pwd`/build/libethaccounts.dylib ./build/Release/eth-account.node')
}