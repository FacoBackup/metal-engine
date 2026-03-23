#ifndef ASSISTANTMESSAGEPANEL_H
#define ASSISTANTMESSAGEPANEL_H

#include "editor/ui/abstract/AbstractPanel.h"
#include "../../dto/ChatDTO.h"
#include "editor/enum/AIModel.h"
#include <map>

namespace Metal {
    class ThemeService;
    class AIAssistantService;
    struct AIAssistantRepository;
    class ImGuiService;

    class AssistantMessagePanel final : public AbstractPanel {
    public:
        AssistantMessagePanel(std::string& currentChatId, AIModel& currentModel);

        void onSync() override;
        void setMessage(ChatMessage* msg, int index, float maxWidth);

        std::vector<Dependency> getDependencies() override {
            return {
                {"ThemeService", &themeService},
                {"AIAssistantService", &aiAssistantService},
                {"AIAssistantRepository", &aiAssistantRepository},
                {"ImGuiService", &imGuiService}
            };
        }

    private:
        ThemeService* themeService = nullptr;
        AIAssistantService* aiAssistantService = nullptr;
        AIAssistantRepository* aiAssistantRepository = nullptr;
        ImGuiService* imGuiService = nullptr;

        ChatMessage* message = nullptr;
        int messageIndex = -1;
        float maxBubbleWidth = 0.0f;
        std::string& currentChatId;
        AIModel& currentModel;
        std::map<std::string, bool> toolAccordionsOpen;

        void renderMessageActions();
        void renderToolCalls(const std::string& childId);
    };
}

#endif
