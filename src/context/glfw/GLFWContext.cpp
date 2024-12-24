#include <imgui_impl_glfw.h>
#include "GLFWContext.h"
#include "../ApplicationContext.h"
#include "../../util/VulkanUtils.h"

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
            (swapChainRebuild || context.vulkanContext.imguiVulkanWindow.Width !=
             fb_width ||
             context.vulkanContext.imguiVulkanWindow.Height != fb_height)) {
            ImGui_ImplVulkan_SetMinImageCount(MAX_FRAMES_IN_FLIGHT);
            ImGui_ImplVulkanH_CreateOrResizeWindow(context.vulkanContext.instance.instance,
                                                   context.vulkanContext.physDevice.physical_device,
                                                   context.vulkanContext.device.device,
                                                   &context.vulkanContext.imguiVulkanWindow,
                                                   context.vulkanContext.queueFamily,
                                                   nullptr, fb_width,
                                                   fb_height,
                                                   MAX_FRAMES_IN_FLIGHT);
            context.vulkanContext.imguiVulkanWindow.FrameIndex = 0;
            swapChainRebuild = false;
        }
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            return false;
        }
        return true;
    }

    void GLFWContext::dispose() const {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    ImGui_ImplVulkanH_Window &GLFWContext::getGUIWindow() const {
        return context.vulkanContext.imguiVulkanWindow;
    }

    void GLFWContext::setSwapChainRebuild(const bool val) {
        swapChainRebuild = val;
    }

    void GLFWContext::presentFrame() {
        if (swapChainRebuild) {
            return;
        }
        auto &wd = context.vulkanContext.imguiVulkanWindow;
        VkSemaphore semaphore = wd.FrameSemaphores[wd.SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &semaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &wd.Swapchain;
        info.pImageIndices = &wd.FrameIndex;
        VkResult err = vkQueuePresentKHR(context.vulkanContext.graphicsQueue, &info);
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
                uint32_t extensions_count = 0;
                const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
                for (uint32_t i = 0; i < extensions_count; i++)
                    instance_extensions.push_back(glfw_extensions[i]);
            }
        }
    }

    const ImVector<const char *> &GLFWContext::getInstanceExtensions() const {
        return instance_extensions;
    }
}
