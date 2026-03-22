#include "UserMessagePanel.h"
#include "../../service/ThemeService.h"
#include "../../service/AIAssistantService.h"
#include "../../repository/AIAssistantRepository.h"
#include "../../util/UIUtil.h"
#include "../../../common/Icons.h"
#include "AIAssistantPanel.h"

namespace Metal {
    UserMessagePanel::UserMessagePanel(std::string &currentChatId, AIModel &currentModel)
        : currentChatId(currentChatId), currentModel(currentModel) {
    }

    void UserMessagePanel::setMessage(ChatMessage *msg, int index, float maxWidth) {
        message = msg;
        messageIndex = index;
        maxBubbleWidth = maxWidth;
    }

    void UserMessagePanel::onSync() {
        if (!message) return;

        float availWidth = ImGui::GetContentRegionAvail().x;
        float wrapWidth = maxBubbleWidth - MCP_MESSAGE_PADDING * 2;
        ImVec2 textSize = ImGui::CalcTextSize(message->content.c_str(), nullptr, false, wrapWidth);
        float bubbleWidth = std::min(maxBubbleWidth,
                                     textSize.x + MCP_MESSAGE_PADDING * 2 + MCP_USER_BUBBLE_EXTRA_WIDTH);
        
        float posX = availWidth - bubbleWidth - MCP_PADDING;
        if (posX < 0) posX = MCP_PADDING;
        ImGui::SetCursorPosX(posX);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, themeService->palette1);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, MCP_MESSAGE_ROUNDING);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(MCP_MESSAGE_PADDING, MCP_MESSAGE_PADDING));

        std::string childId = "msg_user_" + std::to_string(messageIndex) + "_" + message->timestamp + id;

        // Use stored renderedHeight if available, otherwise use initial estimate
        float childHeight = (message->renderedHeight > 0.0f)
                                ? message->renderedHeight
                                : (textSize.y + MCP_MESSAGE_HEADER_HEIGHT + 20.0f);
        if (isEditing) childHeight += MCP_EDITING_EXTRA_HEIGHT;

        if (ImGui::BeginChild(childId.c_str(), ImVec2(bubbleWidth, childHeight), true,
                              ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                              ImGuiWindowFlags_AlwaysUseWindowPadding)) {
            ImGui::TextColored(MCP_SECONDARY_TEXT_COLOR, "[%s] You", message->timestamp.c_str());

            ImGui::SameLine(ImGui::GetContentRegionAvail().x - MCP_ACTIONS_OFFSET);
            renderMessageActions();

            ImGui::Separator();

            if (isEditing) {
                ImGui::InputTextMultiline(("##edit_" + std::to_string(messageIndex)).c_str(), editBuffer,
                                          sizeof(editBuffer), ImVec2(-1, MCP_EDIT_INPUT_HEIGHT));
                if (ImGui::Button("Save")) {
                    aiAssistantService->editMessage(currentChatId, messageIndex, editBuffer, currentModel);
                    isEditing = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    isEditing = false;
                }
            } else {
                ImGui::TextWrapped("%s", message->content.c_str());
            }

            // Measure actual height and update DTO on the next frame if needed
            float actualHeight = ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y;
            if (std::abs(actualHeight - message->renderedHeight) > 1.0f && !isEditing) {
                auto chat = aiAssistantRepository->findChatById(currentChatId);
                if (chat && messageIndex < chat->messages.size()) {
                    chat->messages[messageIndex].renderedHeight = actualHeight;
                }
            }
        }
        ImGui::EndChild();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
    }

    void UserMessagePanel::renderMessageActions() {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

        if (UIUtil::RenderButtonSolid(id + "edit" + std::to_string(messageIndex), Icons::edit, MCP_BUTTON_SIZE,
                                      themeService->palette1, MCP_BUTTON_ROUNDING)) {
            isEditing = true;
            strncpy_s(editBuffer, message->content.c_str(), sizeof(editBuffer));
        }
        ImGui::SameLine();

        if (UIUtil::RenderButtonSolid(id + "del" + std::to_string(messageIndex), Icons::delete_forever, MCP_BUTTON_SIZE,
                                      themeService->palette1, MCP_BUTTON_ROUNDING)) {
            aiAssistantService->deleteMessagesFrom(currentChatId, messageIndex);
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
}
