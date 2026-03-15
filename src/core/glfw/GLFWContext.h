#ifndef METAL_ENGINE_GLFWCONTEXT_H
#define METAL_ENGINE_GLFWCONTEXT_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_internal.h>

#include "../../common/IContextMember.h"
#include "../../common/IDisposable.h"
#include "../../common/IInit.h"


#include <functional>
#include <vector>

struct ImGui_ImplVulkanH_Window;

namespace Metal {
    class ApplicationContext;
    class VulkanContext;
    class ThemeService;

    class GLFWContext final : public IContextMember, public IInit {
        VulkanContext *vulkanContext = nullptr;
        ThemeService *themeService = nullptr;
        GLFWwindow *window = nullptr;
        bool validContext = true;
        bool swapChainRebuild = false;
        ImVector<const char *> instance_extensions{};

        std::vector<std::function<void(int button, int action, int mods)>> mouseButtonCallbacks;
        std::vector<std::function<void(double xpos, double ypos)>> cursorPosCallbacks;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"VulkanContext", &vulkanContext}, {"ThemeService", &themeService}};
        }

        void setSwapChainRebuild(bool val);

        void presentFrame();

        [[nodiscard]] const ImVector<const char *> &getInstanceExtensions() const;

        void onInitialize() override;

        [[nodiscard]] GLFWwindow *getWindow() const;

        [[nodiscard]] bool isValidContext() const;

        bool beginFrame();

        void disposeManually();

        [[nodiscard]] ImGui_ImplVulkanH_Window &getGUIWindow() const;

        void addMouseButtonCallback(std::function<void(int button, int action, int mods)> callback) {
            mouseButtonCallbacks.push_back(std::move(callback));
        }

        void addCursorPosCallback(std::function<void(double xpos, double ypos)> callback) {
            cursorPosCallbacks.push_back(std::move(callback));
        }

        const std::vector<std::function<void(int, int, int)>> &getMouseButtonCallbacks() const {
            return mouseButtonCallbacks;
        }

        const std::vector<std::function<void(double, double)>> &getCursorPosCallbacks() const {
            return cursorPosCallbacks;
        }
    };
}

#endif
