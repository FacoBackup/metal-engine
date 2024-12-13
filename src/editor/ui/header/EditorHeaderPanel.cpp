#include "EditorHeaderPanel.h"
#include "GlobalSettingsPanel.h"
#include "../../common/UIUtil.h"
#include "../../../context/ApplicationContext.h"

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
        if (ImGui::BeginMainMenuBar()) {
            if (UIUtil::ButtonSimple(Icons::save, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
                // Action for save
            }
            ImGui::SameLine();
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New")) {
                    // Action for "New"
                }
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    context->updateRootPath(true); // TODO - CLEAN STATE
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    // Action for "Save"
                }
                ImGui::Separator(); // Adds a separator line
                if (ImGui::MenuItem("Exit")) {
                    exit(0);
                }
                ImGui::EndMenu(); // End "File" menu
            }

            // Create an "Edit" menu
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                    // Action for "Undo"
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {
                    // Action for "Redo", disabled here
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "Ctrl+X")) {
                    // Action for "Cut"
                }
                if (ImGui::MenuItem("Copy", "Ctrl+C")) {
                    // Action for "Copy"
                }
                if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                    // Action for "Paste"
                }
                ImGui::EndMenu();
            }

            UIUtil::Spacing();
            ImGui::Text(Icons::inventory_2.c_str());
            ImGui::SameLine();

            if (strcmp(projectName, context->getProjectName().c_str()) != 0) {
                strcpy(projectName, context->getProjectName().c_str());
            }
            ImGui::SetNextItemWidth(150.0f);
            if (ImGui::InputText(id.c_str(), projectName, 128)) {
                context->updateProjectName(projectName);
            }

            UIUtil::DynamicSpacing(150);
            framerate();
            ImGui::EndMainMenuBar();
        }
    }

    void EditorHeaderPanel::framerate() {
        int framerate = static_cast<int>(round(io->Framerate));
        ImGui::Text("%i ms | %i fps", 1000 / framerate, framerate);
    }

    void EditorHeaderPanel::hotKeys() {
        // TODO
    }
}
