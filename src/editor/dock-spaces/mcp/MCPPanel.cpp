#include "MCPPanel.h"
#include <string>
#include "../../service/MCPService.h"

namespace Metal {
    MCPPanel::MCPPanel() {
        memset(inputBuffer, 0, sizeof(inputBuffer));
    }

    void MCPPanel::onSync() {
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##mcp-input", inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (mcpService) {
                mcpService->sendRequest(inputBuffer);
            }
            memset(inputBuffer, 0, sizeof(inputBuffer));
        }
    }
}
