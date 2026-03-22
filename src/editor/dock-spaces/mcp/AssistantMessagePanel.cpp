#include "AssistantMessagePanel.h"
#include <imgui.h>
#include <imgui_markdown.h>
#include "../../service/ThemeService.h"
#include "../../service/AIAssistantService.h"
#include "../../repository/AIAssistantRepository.h"
#include "../../../core/ImGuiService.h"
#include "../../util/UIUtil.h"
#include "../../../common/Icons.h"
#include "AIAssistantPanel.h"

namespace Metal {
    AssistantMessagePanel::AssistantMessagePanel(std::string& currentChatId, AIModel& currentModel)
        : currentChatId(currentChatId), currentModel(currentModel) {}

    void AssistantMessagePanel::setMessage(ChatMessage* msg, int index, float maxWidth) {
        message = msg;
        messageIndex = index;
        maxBubbleWidth = maxWidth;
    }

    void AssistantMessagePanel::onSync() {
        if (!message) return;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, themeService->palette2);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, MCP_MESSAGE_ROUNDING);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(MCP_MESSAGE_PADDING, MCP_MESSAGE_PADDING));

        std::string childId = "msg_ast_" + std::to_string(messageIndex) + "_" + message->timestamp + id;

        // Calculate height based on content
        float wrapWidth = maxBubbleWidth - MCP_MESSAGE_PADDING * 2;
        ImVec2 contentSize = ImGui::CalcTextSize(message->content.c_str(), nullptr, false, wrapWidth);
        
        // Use stored renderedHeight if available, otherwise use initial estimate
        float childHeight = (message->renderedHeight > 0.0f) ? message->renderedHeight : (contentSize.y + MCP_MESSAGE_HEADER_HEIGHT + 20.0f);

        if (ImGui::BeginChild(childId.c_str(), ImVec2(maxBubbleWidth, childHeight), true,
                              ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysUseWindowPadding)) {
                if (message->isProcessing) {
                    ImGui::TextColored(MCP_SECONDARY_TEXT_COLOR, "Assistant is typing...");
                    ImGui::Separator();
                    ImGui::Text("...");
                }else {

                    ImGui::TextColored(MCP_SECONDARY_TEXT_COLOR, "[%s] Assistant", message->timestamp.c_str());

                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - MCP_ACTIONS_OFFSET);
                    renderMessageActions();

                    ImGui::Separator();

                    ImGui::Markdown(message->content.c_str(), message->content.length(), imGuiService->getMarkdownConfig());

                    renderToolCalls(childId);

                    // Measure actual height and update DTO on the next frame if needed
                    float actualHeight = ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y;
                    if (std::abs(actualHeight - message->renderedHeight) > 1.0f) {
                        auto chat = aiAssistantRepository->findChatById(currentChatId);
                        if (chat && messageIndex < chat->messages.size()) {
                            chat->messages[messageIndex].renderedHeight = actualHeight;
                        }
                    }
                }
        }
        ImGui::EndChild();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
    }

    void AssistantMessagePanel::renderMessageActions() {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

        if (UIUtil::RenderButtonSolid(id + "regen" + std::to_string(messageIndex), Icons::refresh, MCP_BUTTON_SIZE,
                                      themeService->palette2, MCP_BUTTON_ROUNDING)) {
            aiAssistantService->regenerateMessage(currentChatId, messageIndex, currentModel);
        }
        ImGui::SameLine();

        if (UIUtil::RenderButtonSolid(id + "del" + std::to_string(messageIndex), Icons::delete_forever, MCP_BUTTON_SIZE,
                                       themeService->palette2, MCP_BUTTON_ROUNDING)) {
            aiAssistantService->deleteMessagesFrom(currentChatId, messageIndex);
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    void AssistantMessagePanel::renderToolCalls(const std::string& childId) {
        if (message->toolCalls.empty()) return;

        ImGui::Spacing();
        for (size_t i = 0; i < message->toolCalls.size(); ++i) {
            const auto& tool = message->toolCalls[i];
            std::string accordionId = childId + "_tool_" + std::to_string(i) + id;
            bool& open = toolAccordionsOpen[accordionId];
            
            std::string label = "Tool Call: " + tool.name;
            if (UIUtil::Accordion(accordionId, label, open, themeService->palette2)) {
                ImGui::TextWrapped("Description: %s", tool.description.c_str());
                ImGui::Separator();
                ImGui::TextWrapped("Result: %s", tool.result.c_str());
                UIUtil::EndAccordion();
            }
        }
    }
}
