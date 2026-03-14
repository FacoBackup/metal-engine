#include "EditorHeaderPanel.h"
#include "AsyncTaskPanel.h"
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

        // Window dragging logic
        if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            GLFWwindow* window = glfwContext->getWindow();
            int x, y;
            glfwGetWindowPos(window, &x, &y);
            glfwSetWindowPos(window, x + (int)delta.x, y + (int)delta.y);
        }
    }

    void EditorHeaderPanel::renderWindowControls() {
        ImGui::SameLine(ImGui::GetWindowWidth() - (UIUtil::ONLY_ICON_BUTTON_SIZE * 3 + 10));
        
        GLFWwindow* window = glfwContext->getWindow();

        if (UIUtil::ButtonSimple(Icons::minimize, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            glfwIconifyWindow(window);
        }
        ImGui::SameLine();

        bool isMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
        if (UIUtil::ButtonSimple(isMaximized ? Icons::filter_none : Icons::crop_square, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            if (isMaximized) {
                glfwRestoreWindow(window);
            } else {
                glfwMaximizeWindow(window);
            }
        }
        ImGui::SameLine();

        if (UIUtil::ButtonSimple(Icons::close, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
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

            // Create an "Edit" menu
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Compile shaders")) {
                    engineContext->dispose();
                    // TODO - CREATE NEW FRAME
                }
                ImGui::EndMenu();
            }

            UIUtil::LargeSpacing();

            asyncTask->onSync();

            renderWindowControls();

            ImGui::EndMainMenuBar();
        }
    }

    void EditorHeaderPanel::onInitialize() {
        appendChild(asyncTask = new AsyncTaskPanel());
    }
}
