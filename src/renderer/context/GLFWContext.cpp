#include <imgui_impl_glfw.h>
#include "GLFWContext.h"
#include "../VulkanUtils.h"
#include "../../common/runtime/ApplicationContext.h"

namespace Metal {
    GLFWwindow *GLFWContext::getWindow() const {
        return window;
    }

    bool GLFWContext::isValidContext() const {
        return validContext;
    }

    bool GLFWContext::beginFrame() const {
        glfwPollEvents();

        // Resize swap chain?
        int fb_width, fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        if (fb_width > 0 && fb_height > 0 &&
            (context.getVulkanContext().g_SwapChainRebuild || context.getVulkanContext().g_MainWindowData.Width != fb_width ||
             context.getVulkanContext().g_MainWindowData.Height != fb_height)) {
            ImGui_ImplVulkan_SetMinImageCount(context.getVulkanContext().g_MinImageCount);
            ImGui_ImplVulkanH_CreateOrResizeWindow(context.getVulkanContext().g_Instance, context.getVulkanContext().g_PhysicalDevice, context.getVulkanContext().g_Device,
                                                   &context.getVulkanContext().g_MainWindowData,
                                                   context.getVulkanContext().g_QueueFamily, context.getVulkanContext().g_Allocator, fb_width, fb_height,
                                                   context.getVulkanContext().g_MinImageCount);
            context.getVulkanContext().g_MainWindowData.FrameIndex = 0;
            context.getVulkanContext().g_SwapChainRebuild = false;
        }
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            return true;
        }
        return false;
    }

    void GLFWContext::shutdown() const {
        context.getVulkanContext().shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    ImGui_ImplVulkanH_Window &GLFWContext::getGUIWindow() const {
        return context.getVulkanContext().g_MainWindowData;
    }

    void GLFWContext::build(const bool debugMode) {
        glfwSetErrorCallback(VulkanUtils::glfw_error_callback);
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
                context.getVulkanContext().build(debugMode);

                context.getVulkanContext().setupVulkan();
                context.getVulkanContext().setupVulkanWindow(window);
            }
        }
    }
}