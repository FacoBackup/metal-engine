#ifndef CHATMESSAGESPANEL_H
#define CHATMESSAGESPANEL_H

#include "editor/ui/abstract/AbstractPanel.h"
#include "../../enum/AIModel.h"
#include <string>

namespace Metal {
    class AIAssistantService;
    struct AIAssistantRepository;
    class ThemeService;
    class ImGuiService;
    struct ChatMessage;
    class UserMessagePanel;
    class AssistantMessagePanel;

    class ChatMessagesPanel final : public AbstractPanel {
    public:
        ChatMessagesPanel(std::string &currentChatId, AIModel &currentModel);

        void onSync() override;

        void onInitialize() override;

        std::vector<Dependency> getDependencies() override {
            return {
                {"AIAssistantService", &aiAssistantService},
                {"AIAssistantRepository", &aiAssistantRepository},
                {"ThemeService", &themeService},
                {"ImGuiService", &imGuiService}
            };
        }

    private:
        AIAssistantService *aiAssistantService = nullptr;
        AIAssistantRepository *aiAssistantRepository = nullptr;
        ThemeService *themeService = nullptr;
        ImGuiService *imGuiService = nullptr;

        std::string &currentChatId;
        AIModel &currentModel;
        std::string lastChatId;
        int lastMessageCount = 0;
        bool shouldScrollToBottom = false;

        std::shared_ptr<UserMessagePanel> userMessagePanel;
        std::shared_ptr<AssistantMessagePanel> assistantMessagePanel;

        void renderMessages();

        void renderEmptyState();
    };
}

#endif
