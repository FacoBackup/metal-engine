#include "ConsolePanel.h"

#include "imgui.h"
#include "../../../ApplicationContext.h"
#include "../../../common/Icons.h"

namespace Metal {
    ConsolePanel::ConsolePanel() {
        for (bool &i: levelFilter) {
            i = true;
        }
        shortcuts = {
            ShortcutDTO("Clear Logs", ImGuiMod_Ctrl | ImGuiKey_L, [this]() {
                LogService::clear();
            })
        };
    }

    void ConsolePanel::onSync() {
        // Header
        if (ImGui::Button((Icons::clear_all + id + "Clear_Logs").c_str())) {
            LogService::clear();
        }
        ImGui::SameLine();
        filter.Draw(Icons::search.c_str(), 180);
        ImGui::SameLine();

        // Log level filter
        if (ImGui::BeginCombo((id + "LogLevel").c_str(), "Filters")) {
            for (int i = 0; i <= (int) LogLevel::Fatal; i++) {
                LogLevel level = (LogLevel) i;
                ImGui::Selectable(LogService::getLevelName(level), &levelFilter[i],
                                  ImGuiSelectableFlags_DontClosePopups);
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        // Log content
        const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild((id + "ScrollingRegion").c_str(), ImVec2(0, -footerHeightToReserve), false,
                          ImGuiWindowFlags_HorizontalScrollbar);

        const auto entries = LogService::getEntriesSnapshot();
        for (const auto &entry: entries) {
            if (!levelFilter[(int) entry.level]) continue;
            if (!filter.PassFilter(entry.message.c_str())) continue;
            uint32_t levelColor = LogService::getLevelColor(entry.level);
            if (levelColor != 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, levelColor);
            }
            ImGui::TextUnformatted(entry.timestamp.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted("[");
            ImGui::SameLine();
            ImGui::TextUnformatted(LogService::getLevelName(entry.level));
            ImGui::SameLine();
            ImGui::TextUnformatted("]");
            ImGui::SameLine();
            ImGui::TextUnformatted(entry.message.c_str());
            if (levelColor != 0) {
                ImGui::PopStyleColor();
            }
        }

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
        ImGui::Separator();

        // Footer
        ImGui::Checkbox(("Auto-scroll" + id + "autoScroll").c_str(), &autoScroll);
    }
} // Metal
