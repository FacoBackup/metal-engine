#include "AIAssistantPanel.h"
#include <string>
#include "../../service/AIAssistantService.h"
#include "../../service/ThemeService.h"
#include "../../repository/EditorRepository.h"
#include "../../repository/AIAssistantRepository.h"
#include "../../util/UIUtil.h"
#include "../../../common/Icons.h"

namespace Metal {
    AIAssistantPanel::AIAssistantPanel() {
        memset(inputBuffer, 0, sizeof(inputBuffer));
    }

    void AIAssistantPanel::onInitialize() {
        currentChatId = aiAssistantRepository->createNewChat()->id;
    }

    void AIAssistantPanel::onSync() {
        auto currentChat = aiAssistantRepository->findChatById(currentChatId);
        bool hasMessages = currentChat && !currentChat->messages.empty();

        if (!hasMessages) ImGui::BeginDisabled();
        if (UIUtil::RenderButtonSolid("##mcp-new-chat" + id, Icons::add, MCP_BUTTON_SIZE, MCP_ADD_BUTTON_COLOR,
                                      MCP_BUTTON_ROUNDING)) {
            currentChatId = aiAssistantRepository->createNewChat()->id;
        }
        if (!hasMessages) ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button(("History##" + id).c_str())) {
            ImGui::OpenPopup(("ChatHistoryPopup" + id).c_str());
        }

        ImGui::SameLine();
        if (!hasMessages) ImGui::BeginDisabled();
        if (UIUtil::RenderButtonSolid("##mcp-delete-chat" + id, Icons::delete_forever, MCP_BUTTON_SIZE,
                                      MCP_DELETE_BUTTON_COLOR, MCP_BUTTON_ROUNDING)) {
            currentChatId = aiAssistantService->deleteCurrentChat(currentChatId);
        }
        if (!hasMessages) ImGui::EndDisabled();

        if (ImGui::BeginPopup(("ChatHistoryPopup" + id).c_str())) {
            for (const auto &chat: aiAssistantRepository->chats) {
                if (!chat) continue;
                std::string label = chat->name + " (" + chat->date + ")";
                if (ImGui::Selectable((label + "##" + chat->id + id).c_str(), currentChatId == chat->id)) {
                    currentChatId = chat->id;
                }
            }
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (ImGui::BeginCombo(("##ModelSelector" + id).c_str(), getAIModelInfo(currentModel).name.c_str())) {
            for (auto model : getAllAIModels()) {
                bool isSelected = (currentModel == model);
                if (ImGui::Selectable(getAIModelInfo(model).name.c_str(), isSelected)) {
                    currentModel = model;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        renderMessages();

        ImGui::Separator();

        renderInput();
    }

    void AIAssistantPanel::renderMessages() {
        float inputHeight = (ImGui::GetFrameHeightWithSpacing() + MCP_PADDING * 2 + MCP_PADDING);
        ImGui::BeginChild(("ChatMessages" + id).c_str(), ImVec2(0, -inputHeight - ImGui::GetStyle().ItemSpacing.y));

        auto currentChat = aiAssistantRepository->findChatById(currentChatId);
        if (currentChat && !currentChat->messages.empty()) {
            for (const auto &msg: currentChat->messages) {
                bool isUser = msg.role == "user";

                ImGui::PushStyleColor(ImGuiCol_ChildBg, isUser ? themeService->palette1 : themeService->palette2);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, MCP_MESSAGE_ROUNDING);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(MCP_MESSAGE_PADDING, MCP_MESSAGE_PADDING));

                std::string childId = "msg_" + msg.timestamp + std::to_string(rand()) + id;

                // Calculate height based on content
                float wrapWidth = ImGui::GetContentRegionAvail().x - 20.0f;
                ImVec2 textSize = ImGui::CalcTextSize(msg.content.c_str(), nullptr, false, wrapWidth);
                float childHeight = textSize.y + 35.0f; // Extra space for header and padding

                if (ImGui::BeginChild(childId.c_str(), ImVec2(0, childHeight), true,
                                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "[%s] %s", msg.timestamp.c_str(),
                                       isUser ? "You" : "Assistant");
                    ImGui::Separator();
                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::TextUnformatted(msg.content.c_str());
                    ImGui::PopTextWrapPos();
                }
                ImGui::EndChild();

                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor();
                ImGui::Spacing();
            }

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
        } else {
            std::string placeholder = "New Chat - type a message to start";
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 textSize = ImGui::CalcTextSize(placeholder.c_str());
            ImGui::SetCursorPos(ImVec2((windowSize.x - textSize.x) * 0.5f, (windowSize.y - textSize.y) * 0.5f));
            ImGui::TextColored(MCP_TEXT_DARK_COLOR, "%s", placeholder.c_str());
        }
        ImGui::EndChild();
    }

    void AIAssistantPanel::renderInput() {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, MCP_INPUT_ROUNDING);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(MCP_PADDING, MCP_PADDING));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, themeService->palette3);

        float inputHeight = ImGui::GetFrameHeightWithSpacing() + MCP_PADDING * 2;
        if (ImGui::BeginChild(("InputSection" + id).c_str(), ImVec2(0, inputHeight), true)) {
            ImGui::SetNextItemWidth(-ImGui::GetFrameHeightWithSpacing() * 2);
            if (ImGui::InputText(("##mcp-input" + id).c_str(), inputBuffer, sizeof(inputBuffer),
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (strlen(inputBuffer) > 0) {
                    aiAssistantService->sendRequest(currentChatId, inputBuffer, currentModel);
                    memset(inputBuffer, 0, sizeof(inputBuffer));
                }
            }
            ImGui::SameLine();
            if (UIUtil::RenderButtonSolid("##mcp-send" + id, Icons::send, MCP_BUTTON_SIZE, editorRepository->accent,
                                          MCP_BUTTON_ROUNDING)) {
                if (strlen(inputBuffer) > 0) {
                    aiAssistantService->sendRequest(currentChatId, inputBuffer, currentModel);
                    memset(inputBuffer, 0, sizeof(inputBuffer));
                }
            }
        }
        ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}
