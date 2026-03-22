#include "ChatInputPanel.h"
#include "../../service/AIAssistantService.h"
#include "../../repository/EditorRepository.h"
#include "../../repository/AIAssistantRepository.h"
#include "../../service/ThemeService.h"
#include "../../util/UIUtil.h"
#include "../../../common/Icons.h"
#include "AIAssistantPanel.h"

namespace Metal {
    ChatInputPanel::ChatInputPanel(std::string& currentChatId, AIModel& currentModel)
        : currentChatId(currentChatId), currentModel(currentModel) {
        memset(inputBuffer, 0, sizeof(inputBuffer));
    }

    void ChatInputPanel::onSync() {
        auto currentChat = aiAssistantRepository->findChatById(currentChatId);
        bool isProcessing = currentChat && currentChat->isProcessing;

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, MCP_INPUT_ROUNDING);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(MCP_PADDING, MCP_PADDING));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, themeService->palette3);

        float inputHeight = ImGui::GetFrameHeightWithSpacing() + MCP_PADDING * 2;
        if (ImGui::BeginChild(("InputSection" + id).c_str(), ImVec2(0, inputHeight), true)) {
            ImGui::SetNextItemWidth(-ImGui::GetFrameHeightWithSpacing() * 2);

            if (isProcessing) ImGui::BeginDisabled();
            if (ImGui::InputText(("##mcp-input" + id).c_str(), inputBuffer, sizeof(inputBuffer),
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (strlen(inputBuffer) > 0) {
                    aiAssistantService->sendRequest(currentChatId, inputBuffer, currentModel);
                    memset(inputBuffer, 0, sizeof(inputBuffer));
                }
            }
            if (isProcessing) ImGui::EndDisabled();

            ImGui::SameLine();

            if (isProcessing) ImGui::BeginDisabled();
            if (UIUtil::RenderButtonSolid("##mcp-send" + id, Icons::send, MCP_BUTTON_SIZE, editorRepository->accent,
                                          MCP_BUTTON_ROUNDING)) {
                if (strlen(inputBuffer) > 0) {
                    aiAssistantService->sendRequest(currentChatId, inputBuffer, currentModel);
                    memset(inputBuffer, 0, sizeof(inputBuffer));
                }
            }
            if (isProcessing) ImGui::EndDisabled();
        }
        ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}
