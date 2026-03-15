#include <imgui_impl_glfw.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#include "GLFWContext.h"
#include "../../ApplicationContext.h"
#include "../vulkan/VulkanUtils.h"
#include "../vulkan/VulkanContext.h"
#include "../../editor/service/ThemeService.h"

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
                    for (auto &callback: ctx->getMouseButtonCallbacks()) {
                        callback(button, action, mods);
                    }
                });
                glfwSetCursorPosCallback(window, [](GLFWwindow *win, double xpos, double ypos) {
                    auto *ctx = static_cast<GLFWContext *>(glfwGetWindowUserPointer(win));
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

    const ImVector<const char *> &GLFWContext::getInstanceExtensions() const {
        return instance_extensions;
    }
}
