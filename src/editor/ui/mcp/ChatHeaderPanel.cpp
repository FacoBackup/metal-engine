#include "ChatHeaderPanel.h"
#include "../../service/AIAssistantService.h"
#include "../../repository/AIAssistantRepository.h"
#include "editor/service/ThemeService.h"
#include "editor/ui/UIUtil.h"
#include "common/Icons.h"
#include "AIAssistantPanel.h"

namespace Metal {
    ChatHeaderPanel::ChatHeaderPanel(std::string& currentChatId, AIModel& currentModel)
        : currentChatId(currentChatId), currentModel(currentModel) {}

    void ChatHeaderPanel::onSync() {
        if (UIUtil::RenderButtonSolid("##mcp-new-chat" + id, Icons::add, MCP_BUTTON_SIZE, MCP_ADD_BUTTON_COLOR,
                                      MCP_BUTTON_ROUNDING)) {
            currentChatId = aiAssistantRepository->createNewChat()->id;
        }

        ImGui::SameLine();
        if (ImGui::Button(("History##" + id).c_str())) {
            ImGui::OpenPopup(("ChatHistoryPopup" + id).c_str());
        }

        ImGui::SameLine();
        if (UIUtil::RenderButtonSolid("##mcp-delete-chat" + id, Icons::delete_forever, MCP_BUTTON_SIZE,
                                      MCP_DELETE_BUTTON_COLOR, MCP_BUTTON_ROUNDING)) {
            currentChatId = aiAssistantService->deleteCurrentChat(currentChatId);
        }

        if (ImGui::BeginPopup(("ChatHistoryPopup" + id).c_str())) {
            for (const auto &chat: aiAssistantRepository->chats) {
                if (!chat) continue;
                std::string label = chat->name + " (" + chat->date + ")";
                if (ImGui::Selectable((label + "##" + chat->id + id).c_str(), currentChatId == chat->id)) {
                    currentChatId = chat->id;
                }
            }
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(MCP_MODEL_SELECTOR_WIDTH);
        if (ImGui::BeginCombo(("##ModelSelector" + id).c_str(), getAIModelInfo(currentModel).name.c_str())) {
            for (auto model: getAllAIModels()) {
                bool isSelected = (currentModel == model);
                if (ImGui::Selectable(getAIModelInfo(model).name.c_str(), isSelected)) {
                    currentModel = model;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
}
