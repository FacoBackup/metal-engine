#ifndef METAL_ENGINE_GLFWCONTEXT_H
#define METAL_ENGINE_GLFWCONTEXT_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <imgui.h>
#include "VulkanContext.h"


namespace Metal {
    class ApplicationContext;

    class GLFWContext {
        GLFWwindow *window = nullptr;
        bool validContext = true;
        ApplicationContext &context;

    public:
        explicit GLFWContext(ApplicationContext &context) : context(context) {}

        void build(bool debugMode);

        [[nodiscard]] GLFWwindow *getWindow() const;

        bool isValidContext() const;

        bool beginFrame() const;

        void shutdown() const;

        ImGui_ImplVulkanH_Window &getGUIWindow() const;
    };
}

#endif
