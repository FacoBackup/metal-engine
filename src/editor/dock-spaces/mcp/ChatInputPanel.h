#ifndef CHATINPUTPANEL_H
#define CHATINPUTPANEL_H

#include "../../abstract/AbstractPanel.h"
#include "../../enum/AIModel.h"
#include <string>

namespace Metal {
    class AIAssistantService;
    struct AIAssistantRepository;
    struct EditorRepository;
    class ThemeService;

    class ChatInputPanel final : public AbstractPanel {
    public:
        ChatInputPanel(std::string& currentChatId, AIModel& currentModel);

        void onSync() override;

        std::vector<Dependency> getDependencies() override {
            return {
                {"AIAssistantService", &aiAssistantService},
                {"AIAssistantRepository", &aiAssistantRepository},
                {"EditorRepository", &editorRepository},
                {"ThemeService", &themeService}
            };
        }

    private:
        AIAssistantService *aiAssistantService = nullptr;
        AIAssistantRepository *aiAssistantRepository = nullptr;
        EditorRepository *editorRepository = nullptr;
        ThemeService *themeService = nullptr;

        std::string& currentChatId;
        AIModel& currentModel;
        char inputBuffer[1024]{};
    };
}

#endif
