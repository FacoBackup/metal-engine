#include "ConsolePanel.h"

#include "imgui.h"
#include "../../../ApplicationContext.h"
#include "../../../../common/interface/Icons.h"

namespace Metal {
    ConsolePanel::ConsolePanel() {
        for (bool & i : levelFilter) {
            i = true;
        }
    }

    void ConsolePanel::onSync() {
        LogService& logService = context->logService;

        // Header
        if (ImGui::Button("Clear")) {
            logService.clear();
        }
        ImGui::SameLine();
        bool copyToClipboard = ImGui::Button("Copy");
        ImGui::SameLine();
        filter.Draw(Icons::search.c_str(), 180);
        ImGui::SameLine();

        // Log level filter
        if (ImGui::BeginCombo((id + "##LogLevel").c_str(), "Filters")) {
            for (int i = 0; i <= (int)LogLevel::Fatal; i++) {
                LogLevel level = (LogLevel)i;
                ImGui::Selectable(LogService::getLevelName(level), &levelFilter[i], ImGuiSelectableFlags_DontClosePopups);
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        // Log content
        const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (copyToClipboard) {
            ImGui::LogToClipboard();
        }

        const auto entries = logService.getEntriesSnapshot();
        for (const auto& entry : entries) {
            if (!levelFilter[(int)entry.level]) continue;
            if (!filter.PassFilter(entry.message.c_str())) continue;

            ImGui::PushStyleColor(ImGuiCol_Text, LogService::getLevelColor(entry.level));
            ImGui::TextUnformatted(entry.timestamp.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted("[");
            ImGui::SameLine();
            ImGui::TextUnformatted(LogService::getLevelName(entry.level));
            ImGui::SameLine();
            ImGui::TextUnformatted("]");
            ImGui::SameLine();
            ImGui::TextUnformatted(entry.message.c_str());
            ImGui::PopStyleColor();
        }

        if (copyToClipboard) {
            ImGui::LogFinish();
        }

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
        ImGui::Separator();

        // Footer
        ImGui::Checkbox("Auto-scroll", &autoScroll);
    }
} // Metal
