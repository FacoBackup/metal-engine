#include "ChatMessagesPanel.h"
#include "../../service/AIAssistantService.h"
#include "../../service/ThemeService.h"
#include "../../repository/AIAssistantRepository.h"
#include "AIAssistantPanel.h"
#include "UserMessagePanel.h"
#include "AssistantMessagePanel.h"

namespace Metal {
    ChatMessagesPanel::ChatMessagesPanel(std::string& currentChatId, AIModel& currentModel)
        : currentChatId(currentChatId), currentModel(currentModel) {}

    void ChatMessagesPanel::onInitialize() {
        userMessagePanel = initializePanel<UserMessagePanel>(false, currentChatId, currentModel);
        assistantMessagePanel = initializePanel<AssistantMessagePanel>(false, currentChatId, currentModel);
    }

    void ChatMessagesPanel::onSync() {
        renderMessages();
    }

    void ChatMessagesPanel::renderMessages() {
        float inputHeight = (ImGui::GetFrameHeightWithSpacing() + MCP_PADDING * 2 + MCP_PADDING);
        ImGui::BeginChild(("ChatMessages" + id).c_str(), ImVec2(0, -inputHeight - ImGui::GetStyle().ItemSpacing.y));

        auto currentChat = aiAssistantRepository->findChatById(currentChatId);
        if (currentChat && !currentChat->messages.empty()) {
            if (currentChatId != lastChatId) {
                lastChatId = currentChatId;
                shouldScrollToBottom = true;
            }

            if (currentChat->messages.size() != lastMessageCount) {
                lastMessageCount = (int)currentChat->messages.size();
                shouldScrollToBottom = true;
            }

            float maxBubbleWidth = ImGui::GetContentRegionAvail().x * MCP_MAX_BUBBLE_WIDTH_RATIO;

            for (int i = 0; i < currentChat->messages.size(); ++i) {
                auto& msg = currentChat->messages[i];
                if (msg.role == "user") {
                    userMessagePanel->setMessage(&msg, i, maxBubbleWidth);
                    userMessagePanel->onSync();
                } else if (msg.role == "assistant") {
                    assistantMessagePanel->setMessage(&msg, i, maxBubbleWidth);
                    assistantMessagePanel->onSync();
                }
                ImGui::Spacing();
            }

            if (currentChat->isProcessing) {
                shouldScrollToBottom = true;
            }

            if (shouldScrollToBottom) {
                ImGui::SetScrollHereY(1.0f);
                shouldScrollToBottom = false;
            }
        } else {
            renderEmptyState();
        }
        ImGui::EndChild();
    }

    void ChatMessagesPanel::renderEmptyState() {
        std::string placeholder = "New Chat - type a message to start";
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 textSize = ImGui::CalcTextSize(placeholder.c_str());
        ImGui::SetCursorPos(ImVec2((windowSize.x - textSize.x) * 0.5f, (windowSize.y - textSize.y) * 0.5f));
        ImGui::TextColored(MCP_TEXT_DARK_COLOR, "%s", placeholder.c_str());
    }
}
