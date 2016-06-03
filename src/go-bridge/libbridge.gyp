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
						"go build -buildmode=c-shared -o <(SHARED_INTERMEDIATE_DIR)/libbridge bridge.go",
					],
				},
				{
					"action_name": "mv-lib",
					"inputs": ["<(SHARED_INTERMEDIATE_DIR)/libbridge"],
					"outputs": ["<(SHARED_INTERMEDIATE_DIR)/libbridge.so"],
					"action": [
						"eval",
						"mv <(SHARED_INTERMEDIATE_DIR)/libbridge <(SHARED_INTERMEDIATE_DIR)/libbridge.so",
					],
				},
				{
					"action_name": "mv-header",
					"inputs": ["<(SHARED_INTERMEDIATE_DIR)/libbridge.h"],
					"outputs": ["<(SHARED_INTERMEDIATE_DIR)/bridge.h"],
					"action": [
						"eval",
						"mv <(SHARED_INTERMEDIATE_DIR)/libbridge.h <(SHARED_INTERMEDIATE_DIR)/bridge.h",
					],
				}
			]
		},
	]
}

