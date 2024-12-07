#ifndef METAL_ENGINE_GLFWCONTEXT_H
#define METAL_ENGINE_GLFWCONTEXT_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <imgui.h>
#include "VulkanContext.h"


namespace Metal {

    class GLFWContext {
    public:
        void build(bool debugMode);

        GLFWwindow *getWindow() const;

        bool isValidContext() const;

        bool beginFrame();

        VulkanContext &getVulkanContext();

        void shutdown();

        ImGui_ImplVulkanH_Window &getGUIWindow();

    private:
        GLFWwindow *window;
        bool validContext = true;
        VulkanContext context;
    };
}

#endif
