{
	"targets": [
		{
			"target_name": "libbridge",
			"type": "none",
			"description": "Create go-ethereum eth-account package c++ bridge",
						
			"conditions": [
				['OS=="linux"', {
					"actions": [
						{
							"action_name": "generate",
							"inputs": ["bridge.go"],
							"outputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts.h", "<(SHARED_INTERMEDIATE_DIR)/libethaccounts"],
							"action": [
								"eval",
								"go build -x -buildmode=c-shared -o <(SHARED_INTERMEDIATE_DIR)/libethaccounts bridge.go",
							],
						},
						{
							"action_name": "rename-lib",
							"inputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts"],
							"outputs": ["<(PRODUCT_DIR)/libethaccounts.so"],
							"action": [
								"eval",
								"mv <(SHARED_INTERMEDIATE_DIR)/libethaccounts <(PRODUCT_DIR)/libethaccounts.so",
							],
						},
						{
							"action_name": "move-header",
							"inputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts.h"],
							"outputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts.h"],
							"action": [
								"eval",
								"mv <(SHARED_INTERMEDIATE_DIR)/libethaccounts.h <(SHARED_INTERMEDIATE_DIR)/ethaccounts.h",
							],
						},
				
					]
			}],
			['OS=="mac"', {
				"actions": [
				{
					"action_name": "generate",
					"inputs": ["bridge.go"],
					"outputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts.h", "<(SHARED_INTERMEDIATE_DIR)/libethaccounts"],
					"action": [
						"eval",
						"go build -x -buildmode=c-shared -o <(SHARED_INTERMEDIATE_DIR)/libethaccounts bridge.go",
					],
				},
				{
					"action_name": "rename-lib",
					"inputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts"],
					"outputs": ["<(PRODUCT_DIR)/libethaccounts.dylib"],
					"action": [
						"eval",
						"cp <(SHARED_INTERMEDIATE_DIR)/libethaccounts <(PRODUCT_DIR)/libethaccounts.dylib",
					],
				},
				{
					"action_name": "move-header",
					"inputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts.h"],
					"outputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts.h"],
					"action": [
						"eval",
						"mv <(SHARED_INTERMEDIATE_DIR)/libethaccounts.h <(SHARED_INTERMEDIATE_DIR)/ethaccounts.h",
					],
				},
				]
			}]],
		},
	]
}
