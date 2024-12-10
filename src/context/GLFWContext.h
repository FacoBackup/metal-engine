#ifndef METAL_ENGINE_GLFWCONTEXT_H
#define METAL_ENGINE_GLFWCONTEXT_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>
#include "VulkanContext.h"


namespace Metal {
    class ApplicationContext;

    class GLFWContext final : public Initializable{
        GLFWwindow *window = nullptr;
        bool validContext = true;
        ApplicationContext &context;
        bool swapChainRebuild = false;
        ImVector<const char *> instance_extensions{};

    public:
        bool &isSwapChainRebuild();

        void setSwapChainRebuild(bool val);

        explicit GLFWContext(ApplicationContext &context) : context(context) {
        }

        [[nodiscard]] const ImVector<const char *> &getInstanceExtensions() const;

        void onInitialize() override;

        [[nodiscard]] GLFWwindow *getWindow() const;

        bool isValidContext() const;

        bool beginFrame();

        void dispose() const;

        [[nodiscard]] ImGui_ImplVulkanH_Window &getGUIWindow() const;
    };
}

#endif
