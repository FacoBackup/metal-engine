#include "EditorHeaderPanel.h"
#include "../../util/UIUtil.h"
#include "../../../ApplicationContext.h"
#include "../../../core/DirectoryService.h"
#include "../../../core/glfw/GLFWContext.h"
#include "../../../engine/EngineContext.h"

namespace Metal {
    void EditorHeaderPanel::onSync() {
        renderFileTab();
        ImGui::Dummy(ImVec2(0, UIUtil::ONLY_ICON_BUTTON_SIZE));
        ImGui::Dummy(ImVec2(2, 0));
        ImGui::Separator();
    }

    void EditorHeaderPanel::renderWindowControls() {
        ImGui::SameLine(ImGui::GetWindowWidth() - (UIUtil::ONLY_ICON_BUTTON_SIZE * 3 + 20));

        GLFWwindow *window = glfwContext->getWindow();

        if (UIUtil::ButtonSimple((Icons::minimize + id + "min"), UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            glfwIconifyWindow(window);
        }
        ImGui::SameLine();

        bool isMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
        if (UIUtil::ButtonSimple((isMaximized ? Icons::filter_none : Icons::crop_square) + id + "max",
                                 UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            if (isMaximized) {
                glfwRestoreWindow(window);
            } else {
                glfwMaximizeWindow(window);
            }
        }
        ImGui::SameLine();

        if (UIUtil::ButtonSimple(Icons::close + id + "close", UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    void EditorHeaderPanel::renderFileTab() {
        if (ImGui::BeginMainMenuBar()) {
            if (UIUtil::ButtonSimple(Icons::save, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
                directoryService->save();
            }
            ImGui::SameLine();
            if (ImGui::BeginMenu("File")) {
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
                ImGui::EndMenu();
            }

            // Edit menu
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Compile shaders")) {
                    engineContext->dispose();
                }
                ImGui::EndMenu();
            }


            // Window controls (minimize, maximize, close)
            renderWindowControls();

            ImGui::EndMainMenuBar();
        }
    }
}
