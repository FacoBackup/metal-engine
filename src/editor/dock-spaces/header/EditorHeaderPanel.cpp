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

            // --- Drag handle area ---
            // Use remaining width (minus the space reserved for window controls)
            float controlsWidth = UIUtil::ONLY_ICON_BUTTON_SIZE * 3 + 20; // approximate width of minimize/maximize/close
            float dragHandleWidth = ImGui::GetContentRegionAvail().x - controlsWidth;
            if (dragHandleWidth > 0) {
                ImGui::SameLine();
                dragHandleScreenPos = ImGui::GetCursorScreenPos();
                ImGui::InvisibleButton("##header_drag_handle", ImVec2(dragHandleWidth, ImGui::GetFrameHeight()));

                GLFWwindow *window = glfwContext->getWindow();
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    if (!glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) {
                        isDragging = true;
                        dragStartMousePos = ImGui::GetIO().MousePos;
                        glfwGetWindowPos(window, &dragStartWindowX, &dragStartWindowY);
                        lastWindowX = dragStartWindowX;
                        lastWindowY = dragStartWindowY;
                    }
                }

                if (isDragging) {
                    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                        ImVec2 currentMousePos = ImGui::GetIO().MousePos;

                        // Calculate how much the mouse has moved relative to the drag handle's CURRENT screen position
                        // This compensates for the window's own movement
                        float deltaX = currentMousePos.x - dragStartMousePos.x;
                        float deltaY = currentMousePos.y - dragStartMousePos.y;

                        int newWindowX = lastWindowX + (int)deltaX;
                        int newWindowY = lastWindowY + (int)deltaY;

                        if (newWindowX != lastWindowX || newWindowY != lastWindowY) {
                            glfwSetWindowPos(window, newWindowX, newWindowY);
                            lastWindowX = newWindowX;
                            lastWindowY = newWindowY;
                        }
                    } else {
                        isDragging = false;
                    }
                }
            }

            // Window controls (minimize, maximize, close)
            renderWindowControls();

            ImGui::EndMainMenuBar();
        }
    }
}
