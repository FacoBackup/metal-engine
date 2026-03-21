#include "MCPService.h"
#include "../repository/EditorRepository.h"
#include "../../common/LoggerUtil.h"

namespace Metal {
    void MCPService::onInitialize() {
        LOG_INFO("MCPService initialized");
    }

    void MCPService::sendRequest(const std::string& message) {
        if (!editorRepository) {
            LOG_ERROR("EditorRepository not available in MCPService");
            return;
        }
        
        LOG_INFO("Sending MCP request to: " + editorRepository->mcpModel);
        LOG_INFO("Message: " + message);
        
        // For now, just a placeholder as per instructions
    }
}
