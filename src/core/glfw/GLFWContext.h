#ifndef METAL_ENGINE_GLFWCONTEXT_H
#define METAL_ENGINE_GLFWCONTEXT_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>

#include "../../common/IContextMember.h"
#include "../../common/IDisposable.h"
#include "../../common/IInit.h"


struct ImGui_ImplVulkanH_Window;

namespace Metal {
    class ApplicationContext;
    class VulkanContext;

    class GLFWContext final : public IContextMember, public IInit, public IDisposable {
        VulkanContext *vulkanContext = nullptr;
        GLFWwindow *window = nullptr;
        bool validContext = true;
        bool swapChainRebuild = false;
        ImVector<const char *> instance_extensions{};

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"VulkanContext", vulkanContext}};
        }

        void setSwapChainRebuild(bool val);

        void presentFrame();

        [[nodiscard]] const ImVector<const char *> &getInstanceExtensions() const;

        void onInitialize() override;

        [[nodiscard]] GLFWwindow *getWindow() const;

        [[nodiscard]] bool isValidContext() const;

        bool beginFrame();

        void dispose() override;

        [[nodiscard]] ImGui_ImplVulkanH_Window &getGUIWindow() const;
    };
}

#endif
