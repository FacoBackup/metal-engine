#include "EditorHeaderPanel.h"
#include "GlobalSettingsPanel.h"
#include "../../../util/UIUtil.h"
#include "../../../../MetalContextProvider.h"

namespace Metal {
    void EditorHeaderPanel::onSync() {
        renderFileTab();
        ImGui::Dummy(ImVec2(0, UIUtil::ONLY_ICON_BUTTON_SIZE));
        ImGui::Dummy(ImVec2(2, 0));
        ImGui::SameLine();
        onSyncChildren();
        ImGui::Separator();
    }

    void EditorHeaderPanel::renderFileTab() {
        if (UIUtil::ButtonSimple(Icons::save, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            ContextProvider::Get().save();
        }
        ImGui::SameLine();
        if (ImGui::Button("File")) {
            ImGui::OpenPopup("FilePopup");
        }
        if (ImGui::BeginPopup("FilePopup")) {
            UIUtil::Spacing();
            ImGui::Text(Icons::inventory_2.c_str());
            ImGui::SameLine();

            if (strcmp(projectName, SINGLETONS.editorRepository.projectName.c_str()) != 0) {
                strcpy(projectName, SINGLETONS.editorRepository.projectName.c_str());
            }

            ImGui::SetNextItemWidth(150.0f);
            if (ImGui::InputText(id.c_str(), projectName, 128)) {
                SINGLETONS.editorRepository.projectName = projectName;
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                ContextProvider::Get().updateRootPath(true); // TODO - CLEAR APPLICATION STATE
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                ContextProvider::Get().save();
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Compile shaders")) {
                SINGLETONS.passesService.dispose();
                SINGLETONS.passesService.onInitialize();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                exit(0);
            }
            ImGui::EndPopup();
        }
    }

    void EditorHeaderPanel::framerate() {
        const int framerate = static_cast<int>(round(ImGui::GetIO().Framerate));
        ImGui::Text("%i ms | %i fps", 1000 / framerate, framerate);
    }
}
