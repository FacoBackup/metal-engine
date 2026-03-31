#include "ChatInputPanel.h"
#include "../../service/AIAssistantService.h"
#include "../../repository/EditorRepository.h"
#include "../../repository/AIAssistantRepository.h"
#include "editor/service/ThemeService.h"
#include "editor/ui/UIUtil.h"
#include "common/Icons.h"
#include "AIAssistantPanel.h"

namespace Metal {
    ChatInputPanel::ChatInputPanel(std::string& currentChatId, AIModel& currentModel)
        : currentChatId(currentChatId), currentModel(currentModel) {
        memset(inputBuffer, 0, sizeof(inputBuffer));
    }

    void ChatInputPanel::onSync() {
        auto currentChat = aiAssistantRepository->findChatById(currentChatId);
        bool isProcessing = currentChat && currentChat->isProcessing;

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, MCP_INPUT_ROUNDING);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(MCP_PADDING, MCP_PADDING));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, themeService->palette3);

        float inputHeight = ImGui::GetFrameHeightWithSpacing() + MCP_PADDING * 2;
        if (ImGui::BeginChild(("InputSection" + id).c_str(), ImVec2(0, inputHeight), true)) {
            if (isProcessing) ImGui::BeginDisabled();
            const Skill* selectedSkill = currentChat ? currentChat->selectedSkill : nullptr;
            std::string buttonLabel = selectedSkill ? selectedSkill->icon + " " + selectedSkill->label : Icons::star_shine;
            ImVec4 buttonColor = selectedSkill ? ImVec4(0.5f, 0.0f, 0.5f, 1.0f) : themeService->palette3;
            ImVec2 buttonSize = selectedSkill ? ImVec2(0, MCP_BUTTON_SIZE) : ImVec2(MCP_BUTTON_SIZE, MCP_BUTTON_SIZE);

            if (UIUtil::RenderButtonSolid("mcp-skill" + id, buttonLabel, buttonSize, buttonColor,
                                          MCP_BUTTON_ROUNDING)) {
                ImGui::OpenPopup(("SkillPopup" + id).c_str());
            }
            if (isProcessing) ImGui::EndDisabled();

            if (ImGui::BeginPopup(("SkillPopup" + id).c_str())) {
                for (auto const& [name, skill] : SKILLS) {
                    if (ImGui::Selectable((skill.icon + " " + skill.label).c_str())) {
                        if (currentChat) {
                            currentChat->setSelectedSkill(&skill);
                        }
                    }
                }
                if (ImGui::Selectable("None")) {
                    if (currentChat) {
                        currentChat->setSelectedSkill(nullptr);
                    }
                }
                ImGui::EndPopup();
            }

            ImGui::SameLine();
            ImGui::SetNextItemWidth(-ImGui::GetFrameHeightWithSpacing() * 2);

            if (isProcessing) ImGui::BeginDisabled();
            if (ImGui::InputText(("##mcp-input" + id).c_str(), inputBuffer, sizeof(inputBuffer),
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (strlen(inputBuffer) > 0) {
                    aiAssistantService->sendRequest(currentChatId, inputBuffer, currentModel);
                    memset(inputBuffer, 0, sizeof(inputBuffer));
                }
            }
            if (isProcessing) ImGui::EndDisabled();

            ImGui::SameLine();

            if (isProcessing) ImGui::BeginDisabled();
            if (UIUtil::RenderButtonSolid("##mcp-send" + id, Icons::send, MCP_BUTTON_SIZE, editorRepository->accent,
                                          MCP_BUTTON_ROUNDING)) {
                if (strlen(inputBuffer) > 0) {
                    aiAssistantService->sendRequest(currentChatId, inputBuffer, currentModel);
                    memset(inputBuffer, 0, sizeof(inputBuffer));
                }
            }
            if (isProcessing) ImGui::EndDisabled();
        }
        ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}
