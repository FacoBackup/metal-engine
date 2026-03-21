#ifndef MCPPANEL_H
#define MCPPANEL_H

#include "../docks/AbstractDockPanel.h"
#include "imgui.h"
#include "../../enum/AIModel.h"

#define MCP_INPUT_ROUNDING 6.0f
#define MCP_PADDING 8.0f
#define MCP_MESSAGE_ROUNDING 8.0f
#define MCP_BUTTON_SIZE 23.0f
#define MCP_BUTTON_ROUNDING 4.0f
#define MCP_MESSAGE_PADDING 10.0f
#define MCP_TEXT_DARK_COLOR ImVec4(0.4f, 0.4f, 0.4f, 1.0f)
#define MCP_ADD_BUTTON_COLOR ImVec4(0.18f, 0.54f, 0.34f, 1.0f) // Sea Green
#define MCP_DELETE_BUTTON_COLOR ImVec4(0.54f, 0.17f, 0.17f, 1.0f) // Dark Red

namespace Metal {
    class AIAssistantService;
    class ThemeService;
    struct AIAssistantRepository;
    struct EditorRepository;

    class AIAssistantPanel final : public AbstractDockPanel {
    public:
        void onSync() override;

        void onInitialize() override;

        std::vector<Dependency> getDependencies() override {
            return {
                {"AIAssistantService", &aiAssistantService},
                {"AIAssistantRepository", &aiAssistantRepository},
                {"ThemeService", &themeService},
                {"EditorRepository", &editorRepository}
            };
        }

        explicit AIAssistantPanel();

    private:
        AIAssistantService *aiAssistantService = nullptr;
        AIAssistantRepository *aiAssistantRepository = nullptr;
        ThemeService *themeService = nullptr;
        EditorRepository *editorRepository = nullptr;
        AIModel currentModel = AIModel::GEMINI_3_FLASH_LITE;
        char inputBuffer[1024]{};
        std::string currentChatId;

        void renderMessages();

        void renderInput();
    };
}

#endif
