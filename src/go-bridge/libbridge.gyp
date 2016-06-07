{
	"targets": [
		{
			"target_name": "libbridge",
			"type": "none",
			"description": "Create go-ethereum eth-account package c++ bridge",
						
			"actions": [
				{
					"action_name": "generate",
					"inputs": ["bridge.go"],
					"outputs": ["<(SHARED_INTERMEDIATE_DIR)/libbridge.h", "<(SHARED_INTERMEDIATE_DIR)/libbridge"],
					"action": [
						"eval",
						"go build -buildmode=c-shared -o <(SHARED_INTERMEDIATE_DIR)/libethaccounts bridge.go",
					],
				},
				{
					"action_name": "rename-lib",
					"inputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts"],
					"outputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts.so"],
					"action": [
						"eval",
						"cp <(SHARED_INTERMEDIATE_DIR)/libethaccounts <(SHARED_INTERMEDIATE_DIR)/libethaccounts.so",
					],
				},
				{
					"action_name": "copy-lib",
					"inputs": ["<(SHARED_INTERMEDIATE_DIR)/libethaccounts"],
					"outputs": ["<(PRODUCT_DIR)/../libethaccounts.so"],
					"action": [
						"eval",
						"cp <(SHARED_INTERMEDIATE_DIR)/libethaccounts.so <(PRODUCT_DIR)/../libethaccounts.so",
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
				}
			]
		},
	]
}

