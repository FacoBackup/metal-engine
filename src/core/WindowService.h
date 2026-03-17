#ifndef METAL_ENGINE_GLFWCONTEXT_H
#define METAL_ENGINE_GLFWCONTEXT_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "../common/IContextMember.h"
#include "../common/IDisposable.h"
#include "../common/IInit.h"


#include <functional>
#include <vector>

struct ImGui_ImplVulkanH_Window;

namespace Metal {
    class ApplicationContext;
    class VulkanContext;
    class ThemeService;

    class WindowService final : public IContextMember, public IInit {
    public:
        struct WindowControlRects {
            SDL_Rect dragArea;
            SDL_Rect minimizeButton;
            SDL_Rect maximizeButton;
            SDL_Rect closeButton;
        };

    private:
        VulkanContext *vulkanContext = nullptr;
        ThemeService *themeService = nullptr;
        SDL_Window *window = nullptr;
        bool validContext = true;
        std::vector<const char *> instance_extensions{};

        std::vector<std::function<void(int button, int action, int mods)> > mouseButtonCallbacks;
        std::vector<std::function<void(double xpos, double ypos)> > cursorPosCallbacks;

        WindowControlRects controlRects{};

    public:
        void setWindowControlRects(const WindowControlRects &rects) {
            controlRects = rects;
        }

        const WindowControlRects &getWindowControlRects() const {
            return controlRects;
        }

        std::vector<Dependency> getDependencies() override {
            return {{"VulkanContext", &vulkanContext}, {"ThemeService", &themeService}};
        }

        [[nodiscard]] const std::vector<const char *> &getInstanceExtensions() const;

        void onInitialize() override;

        [[nodiscard]] SDL_Window *getWindow() const;

        [[nodiscard]] bool isValidContext() const;

        void disposeManually();

        void addMouseButtonCallback(std::function<void(int button, int action, int mods)> callback) {
            mouseButtonCallbacks.push_back(std::move(callback));
        }

        void addCursorPosCallback(std::function<void(double xpos, double ypos)> callback) {
            cursorPosCallbacks.push_back(std::move(callback));
        }

        const std::vector<std::function<void(int, int, int)> > &getMouseButtonCallbacks() const {
            return mouseButtonCallbacks;
        }

        const std::vector<std::function<void(double, double)> > &getCursorPosCallbacks() const {
            return cursorPosCallbacks;
        }

        void close() const;

        [[nodiscard]] bool isMaximized() const;

        void maximize() const;

        void minimize() const;
    };
}

#endif
