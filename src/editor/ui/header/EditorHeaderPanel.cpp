#include "EditorHeaderPanel.h"
#include "GlobalSettingsPanel.h"
#include "../../common/UIUtil.h"

namespace Metal {
    void EditorHeaderPanel::onSync() {
        hotKeys();
        renderFileTab();
        ImGui::Separator();
        onSyncChildren();
    }

    void EditorHeaderPanel::onInitialize() {
        io = &ImGui::GetIO();
        appendChild(new GlobalSettingsPanel());
    }

    void EditorHeaderPanel::renderFileTab() {
        if (ImGui::Button(BUTTON_NEW_PROJECT.c_str())) {

        }
        ImGui::SameLine();

        if (ImGui::Button(BUTTON_OPEN_PROJECT.c_str())) {

        }
        ImGui::SameLine();

        UIUtil::Spacing();
        if (ImGui::Button(BUTTON_SAVE_PROJECT.c_str())) {

        }
        ImGui::SameLine();
        UIUtil::Spacing();

        if (UIUtil::ButtonSimple(BUTTON_FULL_SCREEN, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {

        }
        ImGui::SameLine();
        UIUtil::Spacing();

        if (UIUtil::ButtonSimple(Icons::undo, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) { /* Action for Undo */ }
        ImGui::SameLine();

        if (UIUtil::ButtonSimple(Icons::redo, UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) { /* Action for Redo */ }
        ImGui::SameLine();


        ImGui::SameLine();
        UIUtil::DynamicSpacing(300);

//        ImGui::SetNextItemWidth(200);
//        ImGui::InputText((id + "input").c_str(), pathToProject, ImGuiInputTextFlags_ReadOnly);
//        ImGui::SameLine();
        framerate();
    }

    void EditorHeaderPanel::framerate() {
        int framerate = static_cast<int>(round(io->Framerate));
        ImGui::Text("%i ms | %i fps", 1000 / framerate, framerate);
    }

    void EditorHeaderPanel::hotKeys() {
// TODO
    }
}