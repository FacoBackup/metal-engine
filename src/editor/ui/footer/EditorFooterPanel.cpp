#include "EditorFooterPanel.h"
#include "../header/AsyncTaskPanel.h"
#include "editor/ui/UIUtil.h"
#include "ApplicationContext.h"
#include "../../repository/EditorRepository.h"
#include "editor/service/ThemeService.h"

namespace Metal {
    void EditorFooterPanel::onSync() {
        ImGui::SameLine();
        renderShortcuts();
        ImGui::SameLine(ImGui::GetWindowWidth() - 100);

        framerate();
        ImGui::SameLine();
        asyncTaskPanel->onSync();
    }

    void EditorFooterPanel::onInitialize() {
        asyncTaskPanel = initializePanel<AsyncTaskPanel>(false);
    }

    void EditorFooterPanel::framerate() {
        const int framerate = static_cast<int>(round(ImGui::GetIO().Framerate));
        if (framerate > 0) {
            ImGui::Text("%i ms | %i fps", 1000 / framerate, framerate);
        }
    }

    void EditorFooterPanel::renderShortcuts() {
        auto &shortcuts = editorRepository->focusedShortcuts;
        if (shortcuts.empty()) return;

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2); // Some padding

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
