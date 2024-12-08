#include <imgui_impl_glfw.h>
#include "GLFWContext.h"
#include "ApplicationContext.h"
#include "VulkanContext.h"
#include "../util/VulkanUtils.h"

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
            (swapChainRebuild || context.getVulkanContext().imguiVulkanWindow.Width !=
             fb_width ||
             context.getVulkanContext().imguiVulkanWindow.Height != fb_height)) {
            ImGui_ImplVulkan_SetMinImageCount(IMAGE_COUNT);
            ImGui_ImplVulkanH_CreateOrResizeWindow(context.getVulkanContext().instance.instance,
                                                   context.getVulkanContext().physDevice.physical_device,
                                                   context.getVulkanContext().device.device,
                                                   &context.getVulkanContext().imguiVulkanWindow,
                                                   context.getVulkanContext().queueFamily,
                                                   context.getVulkanContext().instance.allocation_callbacks, fb_width,
                                                   fb_height,
                                                   IMAGE_COUNT);
            context.getVulkanContext().imguiVulkanWindow.FrameIndex = 0;
            swapChainRebuild = false;
        }
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            return true;
        }
        return false;
    }

    void GLFWContext::dispose() const {
        context.getVulkanContext().dispose();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    ImGui_ImplVulkanH_Window &GLFWContext::getGUIWindow() const {
        return context.getVulkanContext().imguiVulkanWindow;
    }

    bool &GLFWContext::isSwapChainRebuild() {
        return swapChainRebuild;
    }

    void GLFWContext::setSwapChainRebuild(const bool val) {
        swapChainRebuild = val;
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
