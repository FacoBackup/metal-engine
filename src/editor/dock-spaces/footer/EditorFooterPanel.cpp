#include "EditorFooterPanel.h"
#include "../../util/UIUtil.h"
#include "../../../ApplicationContext.h"
#include "../../repository/EditorRepository.h"

namespace Metal {
    void EditorFooterPanel::onSync() {
        renderShortcuts();
        framerate();
    }

    void EditorFooterPanel::framerate() {
        const int framerate = static_cast<int>(round(ImGui::GetIO().Framerate));
        if (framerate > 0) {
            ImGui::SameLine(ImGui::GetWindowWidth() - 100);
            ImGui::Text("%i ms | %i fps", 1000 / framerate, framerate);
        }
    }

    void EditorFooterPanel::renderShortcuts() {
        auto &shortcuts = editorRepository->focusedShortcuts;
        if (shortcuts.empty()) return;

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5); // Some padding

        ImGui::Text(editorRepository->focusedWindowName.c_str());
        ImGui::SameLine();
        std::string label;
        for (size_t i = 0; i < std::min(shortcuts.size(), size_t(3)); ++i) {
            if (i > 0) label += " | ";
            label += UIUtil::GetKeyChordName(shortcuts[i].keyChord) + " " + shortcuts[i].name;
        }

        if (ImGui::Button(label.c_str())) {
            ImGui::OpenPopup("AllShortcutsPopupFooter");
        }

        if (ImGui::BeginPopup("AllShortcutsPopupFooter")) {
            if (ImGui::BeginTable("ShortcutsTableFooter", 2,
                                  ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_SizingStretchProp)) {
                ImGui::TableSetupColumn("Key");
                ImGui::TableSetupColumn("Action");
                ImGui::TableHeadersRow();

                for (const auto &shortcut: shortcuts) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(UIUtil::GetKeyChordName(shortcut.keyChord).c_str());
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(shortcut.name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns)) {
                        shortcut.callback();
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndTable();
            }
            ImGui::EndPopup();
        }
    }
}
