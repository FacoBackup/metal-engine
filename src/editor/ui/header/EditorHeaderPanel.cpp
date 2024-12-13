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
            if (ImGui::Button(Icons::save.c_str())) {
                // Action for save
            }

            UIUtil::Spacing();
            ImGui::Text(Icons::file_open.c_str());
            ImGui::SameLine();

            if (!copied) {
                strcpy(projectName, context->getProjectName().c_str());
                copied = true;
            }
            if (ImGui::InputText(id.c_str(), projectName, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
                context->updateProjectName(projectName);
            }
            ImGui::EndMainMenuBar();
        }
        UIUtil::DynamicSpacing(150);
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
