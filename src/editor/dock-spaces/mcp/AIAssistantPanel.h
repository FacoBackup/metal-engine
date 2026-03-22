#ifndef MCPPANEL_H
#define MCPPANEL_H

#include "../docks/AbstractDockPanel.h"
#include "imgui.h"
#include "../../enum/AIModel.h"
#include "ChatHeaderPanel.h"
#include "ChatMessagesPanel.h"
#include "ChatInputPanel.h"

#define MCP_INPUT_ROUNDING 6.0f
#define MCP_PADDING 8.0f
#define MCP_MESSAGE_ROUNDING 8.0f
#define MCP_BUTTON_SIZE 23.0f
#define MCP_BUTTON_ROUNDING 4.0f
#define MCP_MESSAGE_PADDING 4.0f
#define MCP_TEXT_DARK_COLOR ImVec4(0.4f, 0.4f, 0.4f, 1.0f)
#define MCP_ADD_BUTTON_COLOR ImVec4(0.298f, 0.686f, 0.314f, 1.0f) // Sea Green
#define MCP_DELETE_BUTTON_COLOR ImVec4(0.957f, 0.263f, 0.212f, 1.0f)  // Dark Red

#define MCP_MAX_BUBBLE_WIDTH_RATIO 0.75f
#define MCP_USER_BUBBLE_EXTRA_WIDTH 20.0f
#define MCP_MESSAGE_HEADER_HEIGHT 45.0f
#define MCP_EDITING_EXTRA_HEIGHT 60.0f
#define MCP_EDIT_INPUT_HEIGHT 50.0f
#define MCP_ACTIONS_OFFSET 60.0f
#define MCP_PROCESSING_BUBBLE_HEIGHT 60.0f
#define MCP_MODEL_SELECTOR_WIDTH 150.0f
#define MCP_SECONDARY_TEXT_COLOR ImVec4(0.7f, 0.7f, 0.7f, 1.0f)

namespace Metal {
    class AIAssistantService;
    class ThemeService;
    class ImGuiService;
    struct AIAssistantRepository;
    struct EditorRepository;
    struct ChatMessage;

    class AIAssistantPanel final : public AbstractDockPanel {
    public:
        void onSync() override;

        void onInitialize() override;

        std::vector<Dependency> getDependencies() override {
            return {
                {"AIAssistantRepository", &aiAssistantRepository}
            };
        }

    private:
        AIAssistantRepository *aiAssistantRepository = nullptr;
        
        AIModel currentModel = AIModel::GEMINI_3_FLASH_LITE;
        std::string currentChatId;

        std::shared_ptr<ChatHeaderPanel> headerPanel;
        std::shared_ptr<ChatMessagesPanel> messagesPanel;
        std::shared_ptr<ChatInputPanel> inputPanel;
    };
}

#endif
