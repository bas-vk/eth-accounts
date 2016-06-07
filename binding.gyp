{
	"targets": [
		{
			"target_name": "eth-account",
			"sources": ["src/module/async.cpp", "src/module/manager.cpp", "src/module/account.cpp"],
			"include_dirs": ["<(SHARED_INTERMEDIATE_DIR)", "<!(node -e \"require('nan')\")"],
			"libraries": [
				"-lethaccounts", "-L<(PRODUCT_DIR)/../"
			],
			"dependencies": ["src/go-bridge/libbridge.gyp:libbridge"]
		}
	]
}
