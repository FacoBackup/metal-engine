#ifndef METAL_ENGINE_GLFWCONTEXT_H
#define METAL_ENGINE_GLFWCONTEXT_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>
#include "../../common/AbstractRuntimeComponent.h"


struct ImGui_ImplVulkanH_Window;

namespace Metal {
    class ApplicationContext;

    class VulkanContext;

    class GLFWContext final : public Initializable {
        VulkanContext *vulkanContext = nullptr;
        GLFWwindow *window = nullptr;
        bool validContext = true;
        ;
        bool swapChainRebuild = false;
        ImVector<const char *> instance_extensions{};

    public:
        void setSwapChainRebuild(bool val);

        void presentFrame();

        explicit GLFWContext() = default;

        [[nodiscard]] const ImVector<const char *> &getInstanceExtensions() const;

        void onInitialize() override;

        [[nodiscard]] GLFWwindow *getWindow() const;

        [[nodiscard]] bool isValidContext() const;

        bool beginFrame();

        void dispose() const;

        [[nodiscard]] ImGui_ImplVulkanH_Window &getGUIWindow() const;

        void setVulkanContext(VulkanContext *vulkanContext) {
            this->vulkanContext = vulkanContext;
        }
    };
}

#endif
