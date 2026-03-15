#include "EditorHeaderPanel.h"
#include "../../util/UIUtil.h"
#include "../../../core/DirectoryService.h"
#include "../../../core/glfw/GLFWContext.h"
#include "../../../engine/EngineContext.h"
#define HEIGHT 30.0f

namespace Metal {
    void EditorHeaderPanel::onSync() {
        ImGui::SetCursorPosX(5);
        ImGui::SetCursorPosY(0);
        renderFileTab();
    }

    void EditorHeaderPanel::renderMenu(const char *label, std::function<void()> itemsFunc) {
        if (ImGui::Button(label, ImVec2(0, HEIGHT))) {
            ImGui::OpenPopup(label);
        }
        if (ImGui::BeginPopup(label)) {
            itemsFunc();
            ImGui::EndPopup();
        }
    }

    void EditorHeaderPanel::renderFileTab() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

        if (UIUtil::ButtonSimple(Icons::save, HEIGHT, HEIGHT)) {
            directoryService->save();
        }
        ImGui::SameLine();

        ImGui::SetCursorPosY(0);


        renderMenu("File", [this] {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                engineContext->dispose();
                directoryService->updateRootPath(true);
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                directoryService->save();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                exit(0);
            }
        });

        ImGui::SameLine();

        renderMenu("Edit", [this] {
            if (ImGui::MenuItem("Compile shaders")) {
                engineContext->dispose();
            }
        });

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}
