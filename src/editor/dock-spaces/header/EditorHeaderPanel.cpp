#include "EditorHeaderPanel.h"
#include "../../util/UIUtil.h"
#include "../../../ApplicationContext.h"
#include "../../../core/DirectoryService.h"
#include "../../../core/glfw/GLFWContext.h"
#include "../../../engine/EngineContext.h"
#include "editor/service/ThemeService.h"
#define HEIGHT 35.0f

namespace Metal {
    void EditorHeaderPanel::onSync() {
        ImGui::SetCursorPosX(5);
        ImGui::SetCursorPosY(0);
        renderFileTab();
        renderWindowControls();
    }

    void EditorHeaderPanel::renderWindowControls() {
        ImGui::SameLine(ImGui::GetWindowWidth() - (HEIGHT * 3));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, themeService->palette1);
        ImGui::PushStyleColor(ImGuiCol_Button, themeService->palette1);

        GLFWwindow *window = glfwContext->getWindow();

        WindowManagerInfo info;
        info.menuBarHeight = HEIGHT;
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, themeService->palette3);

        if (UIUtil::ButtonSimple((Icons::minimize + id + "min"), HEIGHT, HEIGHT)) {
            glfwIconifyWindow(window);
        }
        info.minimizeBtnRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        ImGui::SameLine();

        bool isMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
        if (UIUtil::ButtonSimple((isMaximized ? Icons::filter_none : Icons::crop_square) + id + "max",
                                 HEIGHT, HEIGHT)) {
            if (isMaximized) {
                glfwRestoreWindow(window);
            } else {
                glfwMaximizeWindow(window);
            }
        }
        ImGui::PopStyleColor();

        info.maximizeBtnRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
        if (UIUtil::ButtonSimple(Icons::close + id + "close", HEIGHT, HEIGHT)) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        info.closeBtnRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        ImGui::PopStyleColor();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);

        int w, h;
        glfwGetWindowSize(window, &w, &h);
        info.windowWidth = w;
        info.windowHeight = h;

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        if (mode) {
            info.screenWidth = mode->width;
            info.screenHeight = mode->height;
        }

        glfwContext->updateWindowManagerInfo(info);
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
