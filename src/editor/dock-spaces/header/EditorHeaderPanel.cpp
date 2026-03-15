#include "EditorHeaderPanel.h"
#include "../../util/UIUtil.h"
#include "../../../core/DirectoryService.h"
#include "../../../core/glfw/GLFWContext.h"
#include "../../../engine/EngineContext.h"
#include "../../service/DockService.h"
#include "editor/service/ThemeService.h"
#define HEIGHT 30.0f
#define BUTTON_SIZE 25.0f

namespace Metal {
    void EditorHeaderPanel::onSync() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::SetCursorPosX(5);
        ImGui::SetCursorPosY(0);
        renderFileTab();
        ImGui::SameLine();
        renderDockAdders();
        ImGui::PopStyleVar();
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
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

        if (UIUtil::ButtonSimple(Icons::save, BUTTON_SIZE, BUTTON_SIZE)) {
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
        ImGui::PopStyleVar();
    }

    void EditorHeaderPanel::renderDockAdders() {
        const float spacing = 2.0f;
        ImGui::PushStyleColor(ImGuiCol_Button, themeService->palette1);

        ImGui::SetCursorPosY(0);

        if (UIUtil::ButtonSimple(Icons::split_scene_left + "##addLeft", BUTTON_SIZE, BUTTON_SIZE)) {
            dockService->addLeftDock();
        }
        UIUtil::RenderTooltip("Add Left Dock");
        ImGui::SameLine(0, spacing);

        if (UIUtil::ButtonSimple(Icons::split_scene_down + "##addBottom", BUTTON_SIZE, BUTTON_SIZE)) {
            dockService->addBottomDock();
        }
        UIUtil::RenderTooltip("Add Bottom Dock");
        ImGui::SameLine(0, spacing);

        if (UIUtil::ButtonSimple(Icons::split_scene_right + "##addRight", BUTTON_SIZE, BUTTON_SIZE)) {
            dockService->addRightDock();
        }
        UIUtil::RenderTooltip("Add Right Dock");
        ImGui::PopStyleColor();
    }
}
