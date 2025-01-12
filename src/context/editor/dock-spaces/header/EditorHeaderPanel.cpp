#include "EditorHeaderPanel.h"
#include "GlobalSettingsPanel.h"
#include "../../../../util/UIUtil.h"
#include "../../../../context/ApplicationContext.h"

namespace Metal {
    void EditorHeaderPanel::onSync() {
        hotKeys();
        renderFileTab();
        ImGui::Dummy(ImVec2(0, UIUtil::ONLY_ICON_BUTTON_SIZE));
        ImGui::Dummy(ImVec2(2, 0));
        ImGui::SameLine();
        onSyncChildren();
        ImGui::Separator();
    }

    void EditorHeaderPanel::onInitialize() {
        appendChild(new GlobalSettingsPanel());
    }

    void EditorHeaderPanel::renderFileTab() {
        if (ImGui::BeginMainMenuBar()) {
            if (UIUtil::ButtonSimple(Icons::save, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
                context->save();
            }
            ImGui::SameLine();
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    context->updateRootPath(true); // TODO - CLEAR APPLICATION STATE
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    context->save();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) {
                    exit(0);
                }
                ImGui::EndMenu();
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
                ImGui::Separator();
                if (ImGui::MenuItem("Compile shaders")) {
                    context->corePipelines.dispose();
                    context->corePipelines.onInitialize();
                }
                ImGui::EndMenu();
            }

            UIUtil::Spacing();
            ImGui::Text(Icons::inventory_2.c_str());
            ImGui::SameLine();

            if (strcmp(projectName, context->editorRepository.projectName.c_str()) != 0) {
                strcpy(projectName, context->editorRepository.projectName.c_str());
            }
            ImGui::SetNextItemWidth(150.0f);
            if (ImGui::InputText(id.c_str(), projectName, 128)) {
                context->editorRepository.projectName = projectName;
            }
            if (context->voxelizationService.isExecutingVoxelization()) {
                UIUtil::DynamicSpacing(115);
                if (UIUtil::ButtonSimple(Icons::apps, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
                    context->voxelizationService.cancelRequest();
                }
                UIUtil::RenderTooltip("Cancel voxelization request");
            } else {
                UIUtil::DynamicSpacing(90);
            }
            framerate();

            ImGui::EndMainMenuBar();
        }
    }

    void EditorHeaderPanel::framerate() {
        const int framerate = static_cast<int>(round(ImGui::GetIO().Framerate));
        ImGui::Text("%i ms | %i fps", 1000 / framerate, framerate);
    }

    void EditorHeaderPanel::hotKeys() {
        // TODO
    }
}
