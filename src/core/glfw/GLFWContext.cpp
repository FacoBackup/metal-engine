#include <imgui_impl_glfw.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#include "GLFWContext.h"
#include "../../ApplicationContext.h"
#include "../vulkan/VulkanUtils.h"
#include "../vulkan/VulkanContext.h"

namespace Metal {
    GLFWwindow *GLFWContext::getWindow() const {
        return window;
    }

    bool GLFWContext::isValidContext() const {
        return validContext;
    }

    bool GLFWContext::beginFrame() {
        glfwPollEvents();

        // Resize swap chain?
        int fb_width, fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        if (fb_width > 0 && fb_height > 0 &&
            (swapChainRebuild || vulkanContext->imguiVulkanWindow.Width !=
             fb_width ||
             vulkanContext->imguiVulkanWindow.Height != fb_height)) {
            ImGui_ImplVulkan_SetMinImageCount(MAX_FRAMES_IN_FLIGHT);
            ImGui_ImplVulkanH_CreateOrResizeWindow(vulkanContext->instance.instance,
                                                   vulkanContext->physDevice.physical_device,
                                                   vulkanContext->device.device,
                                                   &vulkanContext->imguiVulkanWindow,
                                                   vulkanContext->queueFamily,
                                                   nullptr, fb_width,
                                                   fb_height,
                                                   MAX_FRAMES_IN_FLIGHT);
            vulkanContext->imguiVulkanWindow.FrameIndex = 0;
            swapChainRebuild = false;
        }
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            return false;
        }
        return true;
    }

    void GLFWContext::disposeManually() {
        disposeCursors();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    ImGui_ImplVulkanH_Window &GLFWContext::getGUIWindow() const {
        return vulkanContext->imguiVulkanWindow;
    }

    void GLFWContext::setSwapChainRebuild(const bool val) {
        swapChainRebuild = val;
    }

    void GLFWContext::presentFrame() {
        if (swapChainRebuild) {
            return;
        }
        auto &wd = vulkanContext->imguiVulkanWindow;
        VkSemaphore semaphore = wd.FrameSemaphores[wd.SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &semaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &wd.Swapchain;
        info.pImageIndices = &wd.FrameIndex;
        VkResult err = vkQueuePresentKHR(vulkanContext->graphicsQueue, &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            swapChainRebuild = true;
            return;
        }
        VulkanUtils::CheckVKResult(err);
        wd.SemaphoreIndex =
                (wd.SemaphoreIndex + 1) % wd.SemaphoreCount; // Now we can use the next set of semaphores
    }

    void GLFWContext::onInitialize() {
        glfwSetErrorCallback(VulkanUtils::GLFWErrorCallback);
        if (!glfwInit()) {
            validContext = false;
        }

        if (validContext) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
            window = glfwCreateWindow(1280, 720, ENGINE_NAME, nullptr, nullptr);

            if (!glfwVulkanSupported()) {
                validContext = false;
            } else {
                validContext = true;
                unsigned int extensions_count = 0;
                const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
                for (unsigned int i = 0; i < extensions_count; i++)
                    instance_extensions.push_back(glfw_extensions[i]);

                glfwSetWindowUserPointer(window, this);
                glfwSetMouseButtonCallback(window, [](GLFWwindow *win, int button, int action, int mods) {
                    auto *ctx = static_cast<GLFWContext *>(glfwGetWindowUserPointer(win));
                    ctx->handleWindowManagement(button, action, mods);
                    for (auto &callback: ctx->getMouseButtonCallbacks()) {
                        callback(button, action, mods);
                    }
                });
                glfwSetCursorPosCallback(window, [](GLFWwindow *win, double xpos, double ypos) {
                    auto *ctx = static_cast<GLFWContext *>(glfwGetWindowUserPointer(win));
                    ctx->handleCursorPos(xpos, ypos);
                    ctx->updateCursorShape(xpos, ypos);
                    for (auto &callback: ctx->getCursorPosCallbacks()) {
                        callback(xpos, ypos);
                    }
                });

                hResizeCursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
                vResizeCursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
                nwseResizeCursor = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
                neswResizeCursor = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);

                // Enable Windows 11 rounded corners
                HWND hwnd = glfwGetWin32Window(window);
                if (hwnd) {
                    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
                    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));
                }
            }
        }
    }

    void GLFWContext::handleWindowManagement(int button, int action, int mods) {
        if (button != GLFW_MOUSE_BUTTON_LEFT) return;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (action == GLFW_PRESS) {
            resizeRegion = getResizeRegion(xpos, ypos);
            if (resizeRegion != ResizeRegion::None) {
                isDragging = false;
                glfwGetWindowPos(window, &dragStartWindowX, &dragStartWindowY);
                glfwGetWindowSize(window, &dragStartWindowWidth, &dragStartWindowHeight);
                dragStartCursorX = xpos;
                dragStartCursorY = ypos;
                return;
            }

            bool isInTitleBar = ypos >= 0 && ypos < static_cast<double>(windowManagerInfo.menuBarHeight);

            auto isInRect = [](const ImRect &rect, double x, double y) {
                return x >= rect.Min.x && x <= rect.Max.x && y >= rect.Min.y && y <= rect.Max.y;
            };

            bool isOverButton = isInRect(windowManagerInfo.minimizeBtnRect, xpos, ypos) ||
                                isInRect(windowManagerInfo.maximizeBtnRect, xpos, ypos) ||
                                isInRect(windowManagerInfo.closeBtnRect, xpos, ypos);

            if (isInTitleBar && !isOverButton) {
                double currentTime = glfwGetTime();
                if (currentTime - lastClickTime < 0.3) {
                    if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) {
                        glfwRestoreWindow(window);
                    } else {
                        glfwMaximizeWindow(window);
                    }
                    isDragging = false;
                    lastClickTime = 0;
                    return;
                }
                lastClickTime = currentTime;

                isDragging = true;
                dragStartCursorX = xpos;
                dragStartCursorY = ypos;
                glfwGetWindowPos(window, &dragStartWindowX, &dragStartWindowY);

                if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) {
                    int windowWidth, windowHeight;
                    glfwGetWindowSize(window, &windowWidth, &windowHeight);
                    glfwRestoreWindow(window);
                    int restoredWidth, restoredHeight;
                    glfwGetWindowSize(window, &restoredWidth, &restoredHeight);

                    double relativeX = dragStartCursorX / static_cast<double>(windowWidth);
                    dragStartCursorX = relativeX * restoredWidth;
                    dragStartWindowX = static_cast<int>(xpos - dragStartCursorX);
                    dragStartWindowY = static_cast<int>(ypos - dragStartCursorY);
                    glfwSetWindowPos(window, dragStartWindowX, dragStartWindowY);
                }
            }
        } else if (action == GLFW_RELEASE) {
            if (isDragging && currentSnapTarget.active) {
                applySnap(currentSnapTarget);
            }
            isDragging = false;
            resizeRegion = ResizeRegion::None;
            currentSnapTarget.active = false;
        }
    }

    void GLFWContext::handleCursorPos(double xpos, double ypos) {
        if (isDragging) {
            int wx, wy;
            glfwGetWindowPos(window, &wx, &wy);
            int newX = wx + static_cast<int>(xpos - dragStartCursorX);
            int newY = wy + static_cast<int>(ypos - dragStartCursorY);
            glfwSetWindowPos(window, newX, newY);
            currentSnapTarget = calculateSnapTarget(xpos, ypos);
        } else if (resizeRegion != ResizeRegion::None) {
            int dx = static_cast<int>(xpos - dragStartCursorX);
            int dy = static_cast<int>(ypos - dragStartCursorY);

            int newX = dragStartWindowX;
            int newY = dragStartWindowY;
            int newW = dragStartWindowWidth;
            int newH = dragStartWindowHeight;

            const int minW = 400;
            const int minH = 300;

            switch (resizeRegion) {
                case ResizeRegion::Left:
                    newX += dx;
                    newW -= dx;
                    break;
                case ResizeRegion::Right:
                    newW += dx;
                    break;
                case ResizeRegion::Top:
                    newY += dy;
                    newH -= dy;
                    break;
                case ResizeRegion::Bottom:
                    newH += dy;
                    break;
                case ResizeRegion::TopLeft:
                    newX += dx;
                    newW -= dx;
                    newY += dy;
                    newH -= dy;
                    break;
                case ResizeRegion::TopRight:
                    newW += dx;
                    newY += dy;
                    newH -= dy;
                    break;
                case ResizeRegion::BottomLeft:
                    newX += dx;
                    newW -= dx;
                    newH += dy;
                    break;
                case ResizeRegion::BottomRight:
                    newW += dx;
                    newH += dy;
                    break;
                default:
                    break;
            }

            if (newW < minW) {
                if (resizeRegion == ResizeRegion::Left || resizeRegion == ResizeRegion::TopLeft || resizeRegion == ResizeRegion::BottomLeft) {
                    newX = dragStartWindowX + (dragStartWindowWidth - minW);
                }
                newW = minW;
            }
            if (newH < minH) {
                if (resizeRegion == ResizeRegion::Top || resizeRegion == ResizeRegion::TopLeft || resizeRegion == ResizeRegion::TopRight) {
                    newY = dragStartWindowY + (dragStartWindowHeight - minH);
                }
                newH = minH;
            }

            int curX, curY, curW, curH;
            glfwGetWindowPos(window, &curX, &curY);
            glfwGetWindowSize(window, &curW, &curH);

            if (newX != curX || newY != curY) glfwSetWindowPos(window, newX, newY);
            if (newW != curW || newH != curH) glfwSetWindowSize(window, newW, newH);
        }
    }

    void GLFWContext::updateCursorShape(double xpos, double ypos) {
        if (isDragging) return;

        ResizeRegion region = (resizeRegion != ResizeRegion::None) ? resizeRegion : getResizeRegion(xpos, ypos);
        GLFWcursor *cursor = nullptr;

        switch (region) {
            case ResizeRegion::Left:
            case ResizeRegion::Right:
                cursor = hResizeCursor;
                break;
            case ResizeRegion::Top:
            case ResizeRegion::Bottom:
                cursor = vResizeCursor;
                break;
            case ResizeRegion::TopLeft:
            case ResizeRegion::BottomRight:
                cursor = nwseResizeCursor;
                break;
            case ResizeRegion::TopRight:
            case ResizeRegion::BottomLeft:
                cursor = neswResizeCursor;
                break;
            default:
                break;
        }

        glfwSetCursor(window, cursor);
    }

    ResizeRegion GLFWContext::getResizeRegion(double xpos, double ypos) {
        if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) return ResizeRegion::None;

        int w, h;
        glfwGetWindowSize(window, &w, &h);
        const double thickness = 8.0;

        bool left = xpos < thickness;
        bool right = xpos > w - thickness;
        bool top = ypos < thickness;
        bool bottom = ypos > h - thickness;

        if (left && top) return ResizeRegion::TopLeft;
        if (right && top) return ResizeRegion::TopRight;
        if (left && bottom) return ResizeRegion::BottomLeft;
        if (right && bottom) return ResizeRegion::BottomRight;
        if (left) return ResizeRegion::Left;
        if (right) return ResizeRegion::Right;
        if (top) return ResizeRegion::Top;
        if (bottom) return ResizeRegion::Bottom;

        return ResizeRegion::None;
    }

    void GLFWContext::applySnap(const SnapTarget &target) {
        if (target.isMaximized) {
            glfwMaximizeWindow(window);
        } else {
            glfwRestoreWindow(window);
            glfwSetWindowPos(window, target.x, target.y);
            glfwSetWindowSize(window, target.width, target.height);
        }
    }

    SnapTarget GLFWContext::calculateSnapTarget(double xpos, double ypos) {
        SnapTarget target;
        int wx, wy;
        glfwGetWindowPos(window, &wx, &wy);
        double screenX = wx + xpos;
        double screenY = wy + ypos;

        int screenW = windowManagerInfo.screenWidth;
        int screenH = windowManagerInfo.screenHeight;

        const int threshold = 10;

        if (screenY < threshold) {
            target.active = true;
            target.isMaximized = true;
            return target;
        }

        if (screenX < threshold) {
            target.active = true;
            target.x = 0;
            target.y = 0;
            target.width = screenW / 2;
            target.height = screenH;
            if (screenY < threshold * 5) {
                target.height = screenH / 2;
            } else if (screenY > screenH - threshold * 5) {
                target.y = screenH / 2;
                target.height = screenH / 2;
            }
            return target;
        }

        if (screenX > screenW - threshold) {
            target.active = true;
            target.x = screenW / 2;
            target.y = 0;
            target.width = screenW / 2;
            target.height = screenH;
            if (screenY < threshold * 5) {
                target.height = screenH / 2;
            } else if (screenY > screenH - threshold * 5) {
                target.y = screenH / 2;
                target.height = screenH / 2;
            }
            return target;
        }

        return target;
    }

    void GLFWContext::disposeCursors() {
        if (hResizeCursor) glfwDestroyCursor(hResizeCursor);
        if (vResizeCursor) glfwDestroyCursor(vResizeCursor);
        if (nwseResizeCursor) glfwDestroyCursor(nwseResizeCursor);
        if (neswResizeCursor) glfwDestroyCursor(neswResizeCursor);
        hResizeCursor = vResizeCursor = nwseResizeCursor = neswResizeCursor = nullptr;
    }

    const ImVector<const char *> &GLFWContext::getInstanceExtensions() const {
        return instance_extensions;
    }
}
