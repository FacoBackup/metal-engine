#include <imgui_impl_glfw.h>
#include "GLFWContext.h"
#include "../MetalContextProvider.h"
#include "../renderer/util/VulkanUtils.h"

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
            (swapChainRebuild || SINGLETONS.vulkanContext.imguiVulkanWindow.Width !=
             fb_width ||
             SINGLETONS.vulkanContext.imguiVulkanWindow.Height != fb_height)) {
            ImGui_ImplVulkan_SetMinImageCount(MAX_FRAMES_IN_FLIGHT);
            ImGui_ImplVulkanH_CreateOrResizeWindow(SINGLETONS.vulkanContext.instance.instance,
                                                   SINGLETONS.vulkanContext.physDevice.physical_device,
                                                   SINGLETONS.vulkanContext.device.device,
                                                   &SINGLETONS.vulkanContext.imguiVulkanWindow,
                                                   SINGLETONS.vulkanContext.queueFamily,
                                                   nullptr, fb_width,
                                                   fb_height,
                                                   MAX_FRAMES_IN_FLIGHT);
            SINGLETONS.vulkanContext.imguiVulkanWindow.FrameIndex = 0;
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
        return SINGLETONS.vulkanContext.imguiVulkanWindow;
    }

    void GLFWContext::setSwapChainRebuild(const bool val) {
        swapChainRebuild = val;
    }

    void GLFWContext::presentFrame() {
        if (swapChainRebuild) {
            return;
        }
        auto &wd = SINGLETONS.vulkanContext.imguiVulkanWindow;
        VkSemaphore semaphore = wd.FrameSemaphores[wd.SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &semaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &wd.Swapchain;
        info.pImageIndices = &wd.FrameIndex;
        VkResult err = vkQueuePresentKHR(SINGLETONS.vulkanContext.graphicsQueue, &info);
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
            }
        }
    }

    const ImVector<const char *> &GLFWContext::getInstanceExtensions() const {
        return instance_extensions;
    }
}
