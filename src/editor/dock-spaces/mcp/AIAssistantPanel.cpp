#include "AIAssistantPanel.h"
#include <string>
#include <imgui_markdown.h>
#include "../../service/AIAssistantService.h"
#include "../../service/ThemeService.h"
#include "../../../core/ImGuiService.h"
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
            for (int i = 0; i < currentChat->messages.size(); ++i) {
                const auto &msg = currentChat->messages[i];
                bool isUser = msg.role == "user";

                float windowWidth = ImGui::GetWindowWidth();
                float maxBubbleWidth = windowWidth * 0.75f;
                
                // Align user messages to the right
                if (isUser) {
                    float wrapWidth = maxBubbleWidth - MCP_MESSAGE_PADDING * 2;
                    ImVec2 textSize = ImGui::CalcTextSize(msg.content.c_str(), nullptr, false, wrapWidth);
                    float bubbleWidth = std::min(maxBubbleWidth, textSize.x + MCP_MESSAGE_PADDING * 2 + 20.0f);
                    ImGui::SetCursorPosX(windowWidth - bubbleWidth - ImGui::GetStyle().ScrollbarSize - MCP_PADDING);
                }

                ImGui::PushStyleColor(ImGuiCol_ChildBg, isUser ? themeService->palette1 : themeService->palette2);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, MCP_MESSAGE_ROUNDING);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(MCP_MESSAGE_PADDING, MCP_MESSAGE_PADDING));

                std::string childId = "msg_" + std::to_string(i) + "_" + msg.timestamp + id;

                // Calculate height based on content
                float wrapWidth = maxBubbleWidth - MCP_MESSAGE_PADDING * 2;
                ImVec2 contentSize = ImGui::CalcTextSize(msg.content.c_str(), nullptr, false, wrapWidth);
                float childHeight = contentSize.y + 45.0f; // Space for header, actions and padding
                if (editingMessageIndex == i) childHeight += 60.0f; // Extra space for editing UI

                if (ImGui::BeginChild(childId.c_str(), ImVec2(maxBubbleWidth, childHeight), true,
                                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
                    
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "[%s] %s", msg.timestamp.c_str(),
                                       isUser ? "You" : "Assistant");
                    
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 60);
                    renderMessageActions(i, isUser);
                    
                    ImGui::Separator();
                    
                    if (editingMessageIndex == i) {
                        ImGui::InputTextMultiline(("##edit_" + std::to_string(i)).c_str(), editBuffer, sizeof(editBuffer), ImVec2(-1, 50));
                        if (ImGui::Button("Save")) {
                            aiAssistantService->editMessage(currentChatId, i, editBuffer, currentModel);
                            editingMessageIndex = -1;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel")) {
                            editingMessageIndex = -1;
                        }
                    } else {
                        ImGui::Markdown(msg.content.c_str(), msg.content.length(), imGuiService->getMarkdownConfig());
                    }
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

    void AIAssistantPanel::renderMessageActions(int index, bool isUser) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        
        if (isUser) {
            if (ImGui::Button((Icons::edit + std::string("##edit") + std::to_string(index)).c_str())) {
                auto currentChat = aiAssistantRepository->findChatById(currentChatId);
                if (currentChat && index < currentChat->messages.size()) {
                    editingMessageIndex = index;
                    strncpy(editBuffer, currentChat->messages[index].content.c_str(), sizeof(editBuffer));
                }
            }
            ImGui::SameLine();
        } else {
            if (ImGui::Button((Icons::refresh + std::string("##regen") + std::to_string(index)).c_str())) {
                aiAssistantService->regenerateMessage(currentChatId, index, currentModel);
            }
            ImGui::SameLine();
        }

        if (ImGui::Button((Icons::delete_forever + std::string("##del") + std::to_string(index)).c_str())) {
            aiAssistantService->deleteMessagesFrom(currentChatId, index);
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
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
