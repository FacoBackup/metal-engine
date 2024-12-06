#include <imgui_impl_glfw.h>
#include "GLFWContext.h"
#include "../VulkanUtils.h"
#include "../../common/runtime/ApplicationContext.h"
#include "VulkanContext.h"

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
            (context.getVulkanContext().isSwapChainRebuild || context.getVulkanContext().g_MainWindowData.Width !=
             fb_width ||
             context.getVulkanContext().g_MainWindowData.Height != fb_height)) {
            ImGui_ImplVulkan_SetMinImageCount(MIN_IMAGE_COUNT);
            ImGui_ImplVulkanH_CreateOrResizeWindow(context.getVulkanContext().instance.instance,
                                                   context.getVulkanContext().physDevice.physical_device,
                                                   context.getVulkanContext().device.device,
                                                   &context.getVulkanContext().g_MainWindowData,
                                                   context.getVulkanContext().queueFamily,
                                                   context.getVulkanContext().instance.allocation_callbacks, fb_width,
                                                   fb_height,
                                                   MIN_IMAGE_COUNT);
            context.getVulkanContext().g_MainWindowData.FrameIndex = 0;
            context.getVulkanContext().isSwapChainRebuild = false;
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
                context.getVulkanContext().build(debugMode);

                ImVector<const char *> instance_extensions;
                uint32_t extensions_count = 0;
                const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
                for (uint32_t i = 0; i < extensions_count; i++)
                    instance_extensions.push_back(glfw_extensions[i]);

                context.getVulkanContext().setupVulkan(window, instance_extensions);
            }
        }
    }
}
