#ifndef CHATHEADERPANEL_H
#define CHATHEADERPANEL_H

#include "editor/ui/abstract/AbstractPanel.h"
#include "../../enum/AIModel.h"
#include <string>

namespace Metal {
    class AIAssistantService;
    struct AIAssistantRepository;
    class ThemeService;

    class ChatHeaderPanel final : public AbstractPanel {
    public:
        ChatHeaderPanel(std::string& currentChatId, AIModel& currentModel);

        void onSync() override;

        std::vector<Dependency> getDependencies() override {
            return {
                {"AIAssistantService", &aiAssistantService},
                {"AIAssistantRepository", &aiAssistantRepository},
                {"ThemeService", &themeService}
            };
        }

    private:
        AIAssistantService *aiAssistantService = nullptr;
        AIAssistantRepository *aiAssistantRepository = nullptr;
        ThemeService *themeService = nullptr;
        
        std::string& currentChatId;
        AIModel& currentModel;
    };
}

#endif
