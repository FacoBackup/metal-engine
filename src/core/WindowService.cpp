#include <imgui_impl_glfw.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#include "WindowService.h"
#include "../ApplicationContext.h"
#include "../common/VulkanUtils.h"
#include "VulkanContext.h"
#include "../editor/service/ThemeService.h"

namespace Metal {
    GLFWwindow *WindowService::getWindow() const {
        return window;
    }

    bool WindowService::isValidContext() const {
        return validContext;
    }

    void WindowService::disposeManually() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void WindowService::onInitialize() {
        glfwSetErrorCallback(VulkanUtils::GLFWErrorCallback);
        if (!glfwInit()) {
            validContext = false;
        }

        if (validContext) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
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
                    auto *ctx = static_cast<WindowService *>(glfwGetWindowUserPointer(win));
                    for (auto &callback: ctx->getMouseButtonCallbacks()) {
                        callback(button, action, mods);
                    }
                });
                glfwSetCursorPosCallback(window, [](GLFWwindow *win, double xpos, double ypos) {
                    auto *ctx = static_cast<WindowService *>(glfwGetWindowUserPointer(win));
                    for (auto &callback: ctx->getCursorPosCallbacks()) {
                        callback(xpos, ypos);
                    }
                });

                // Enable Windows 11 rounded corners and title bar color
                HWND hwnd = glfwGetWin32Window(window);
                if (hwnd) {
                    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
                    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

                    if (themeService) {
                        COLORREF color = RGB(
                            static_cast<int>(themeService->palette1.x * 255),
                            static_cast<int>(themeService->palette1.y * 255),
                            static_cast<int>(themeService->palette1.z * 255)
                        );
                        DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));

                        COLORREF textColor = RGB(255, 255, 255);
                        DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &textColor, sizeof(textColor));
                    }
                }
            }
        }
    }

    const ImVector<const char *> &WindowService::getInstanceExtensions() const {
        return instance_extensions;
    }
}
