#include "EditorHeaderPanel.h"
#include "AsyncTaskPanel.h"
#include "../../../../util/UIUtil.h"
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
                ApplicationContext::Get().save();
            }
            ImGui::SameLine();
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    ApplicationContext::Get().updateRootPath(true); // TODO - CLEAR APPLICATION STATE
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    ApplicationContext::Get().save();
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
                    ApplicationContext::Get().passesService.dispose();
                    ApplicationContext::Get().passesService.onInitialize();
                }
                ImGui::EndMenu();
            }

            UIUtil::Spacing();


            asyncTask->onSync();

            framerate();

            ImGui::EndMainMenuBar();
        }
    }

    void EditorHeaderPanel::framerate() {
        const int framerate = static_cast<int>(round(ImGui::GetIO().Framerate));
        ImGui::Text("%i ms | %i fps", 1000 / framerate, framerate);
    }

}
