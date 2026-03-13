#include "EditorHeaderPanel.h"
#include "AsyncTaskPanel.h"
#include "../../util/UIUtil.h"
#include "../../../../context/ApplicationContext.h"

namespace Metal {
    void EditorHeaderPanel::onSync() {
        renderFileTab();
        ImGui::Dummy(ImVec2(0, UIUtil::ONLY_ICON_BUTTON_SIZE));
        ImGui::Dummy(ImVec2(2, 0));
        ImGui::Separator();
    }

    void EditorHeaderPanel::onInitialize() {
        appendChild(asyncTask = new AsyncTaskPanel());
    }

    void EditorHeaderPanel::renderFileTab() {
        if (ImGui::BeginMainMenuBar()) {
            if (UIUtil::ButtonSimple(Icons::save, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
                CTX.save();
            }
            ImGui::SameLine();
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    CTX.engineContext.dispose();
                    CTX.updateRootPath(true);
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    CTX.save();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) {
                    exit(0);
                }
                ImGui::EndMenu();
            }

            // Create an "Edit" menu
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Compile shaders")) {
                    CTX.engineContext.dispose();
                    // TODO - CREATE NEW FRAME
                }
                ImGui::EndMenu();
            }

            UIUtil::LargeSpacing();
            renderShortcuts();
            UIUtil::Spacing();

            asyncTask->onSync();

            framerate();

            ImGui::EndMainMenuBar();
        }
    }

    void EditorHeaderPanel::framerate() {
        const int framerate = static_cast<int>(round(ImGui::GetIO().Framerate));
        if (framerate > 0) {
            ImGui::Text("%i ms | %i fps", 1000 / framerate, framerate);
        }
    }

    void EditorHeaderPanel::renderShortcuts() {
        auto &shortcuts = CTX.editorRepository.focusedShortcuts;
        if (shortcuts.empty()) return;
        ImGui::Text(CTX.editorRepository.focusedWindowName.c_str());
        ImGui::SameLine();
        std::string label;
        for (size_t i = 0; i < std::min(shortcuts.size(), size_t(3)); ++i) {
            if (i > 0) label += " | ";
            label += UIUtil::GetKeyChordName(shortcuts[i].keyChord);
        }

        if (ImGui::Button(label.c_str())) {
            ImGui::OpenPopup("AllShortcutsPopup");
        }

        if (ImGui::BeginPopup("AllShortcutsPopup")) {
            if (ImGui::BeginTable("ShortcutsTable", 2,
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
