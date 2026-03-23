#ifndef USERMESSAGEPANEL_H
#define USERMESSAGEPANEL_H

#include "editor/ui/abstract/AbstractPanel.h"
#include "../../dto/ChatDTO.h"
#include "editor/enum/AIModel.h"

namespace Metal {
    class ThemeService;
    class AIAssistantService;
    struct AIAssistantRepository;

    class UserMessagePanel final : public AbstractPanel {
    public:
        UserMessagePanel(std::string &currentChatId, AIModel &currentModel);

        void onSync() override;

        void setMessage(ChatMessage *msg, int index, float maxWidth);

        std::vector<Dependency> getDependencies() override {
            return {
                {"ThemeService", &themeService},
                {"AIAssistantService", &aiAssistantService},
                {"AIAssistantRepository", &aiAssistantRepository}
            };
        }

    private:
        ThemeService *themeService = nullptr;
        AIAssistantService *aiAssistantService = nullptr;
        AIAssistantRepository *aiAssistantRepository = nullptr;

        ChatMessage *message = nullptr;
        int messageIndex = -1;
        float maxBubbleWidth = 0.0f;
        std::string &currentChatId;
        AIModel &currentModel;
        char editBuffer[1024]{};
        bool isEditing = false;

        void renderMessageActions();
    };
}

#endif
