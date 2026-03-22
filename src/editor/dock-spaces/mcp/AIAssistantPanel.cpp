#include "AIAssistantPanel.h"
#include <string>
#include "../../repository/AIAssistantRepository.h"

namespace Metal {
    void AIAssistantPanel::onInitialize() {
        if (!aiAssistantRepository->chats.empty()) {
            currentChatId = aiAssistantRepository->chats.back()->id;
        } else {
            currentChatId = aiAssistantRepository->createNewChat()->id;
        }

        headerPanel = initializePanel<ChatHeaderPanel>(false, currentChatId, currentModel);
        messagesPanel = initializePanel<ChatMessagesPanel>(false, currentChatId, currentModel);
        inputPanel = initializePanel<ChatInputPanel>(false, currentChatId, currentModel);
    }

    void AIAssistantPanel::onSync() {
        headerPanel->onSync();
        ImGui::Separator();
        messagesPanel->onSync();
        ImGui::Separator();
        inputPanel->onSync();
    }
}
