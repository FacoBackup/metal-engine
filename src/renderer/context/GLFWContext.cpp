#include <imgui_impl_glfw.h>
#include "GLFWContext.h"

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
            (context.g_SwapChainRebuild || context.g_MainWindowData.Width != fb_width ||
             context.g_MainWindowData.Height != fb_height)) {
            ImGui_ImplVulkan_SetMinImageCount(context.g_MinImageCount);
            ImGui_ImplVulkanH_CreateOrResizeWindow(context.g_Instance, context.g_PhysicalDevice, context.g_Device,
                                                   &context.g_MainWindowData,
                                                   context.g_QueueFamily, context.g_Allocator, fb_width, fb_height,
                                                   context.g_MinImageCount);
            context.g_MainWindowData.FrameIndex = 0;
            context.g_SwapChainRebuild = false;
        }
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            return true;
        }
        return false;
    }

    VulkanContext &GLFWContext::getVulkanContext() {
        return context;
    }

    void GLFWContext::shutdown() {
        context.shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    ImGui_ImplVulkanH_Window &GLFWContext::getGUIWindow() {
        return context.g_MainWindowData;
    }

    void GLFWContext::build(bool debugMode) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+Vulkan example", nullptr, nullptr);

        if (!glfwVulkanSupported()) {
            validContext = false;
        } else {
            validContext = true;
            context.build(debugMode);
        }
    }
}