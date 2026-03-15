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

    enum class ResizeRegion {
        None,
        Left,
        Right,
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    struct SnapTarget {
        int x, y, width, height;
        bool active = false;
        bool isMaximized = false;
    };

    struct WindowManagerInfo {
        float menuBarHeight = 0;
        ImRect minimizeBtnRect;
        ImRect maximizeBtnRect;
        ImRect closeBtnRect;
        int windowWidth = 0;
        int windowHeight = 0;
        int screenWidth = 0;
        int screenHeight = 0;
    };

    class GLFWContext final : public IContextMember, public IInit {
        VulkanContext *vulkanContext = nullptr;
        GLFWwindow *window = nullptr;
        bool validContext = true;
        bool swapChainRebuild = false;
        ImVector<const char *> instance_extensions{};

        std::vector<std::function<void(int button, int action, int mods)>> mouseButtonCallbacks;
        std::vector<std::function<void(double xpos, double ypos)>> cursorPosCallbacks;

        // Window management state
        WindowManagerInfo windowManagerInfo;
        bool isDragging = false;
        ResizeRegion resizeRegion = ResizeRegion::None;
        double dragStartCursorX = 0;
        double dragStartCursorY = 0;
        int dragStartWindowX = 0;
        int dragStartWindowY = 0;
        int dragStartWindowWidth = 0;
        int dragStartWindowHeight = 0;
        double lastClickTime = 0;

        // Snapping state
        SnapTarget currentSnapTarget;

        // Cursor shapes
        GLFWcursor *hResizeCursor = nullptr;
        GLFWcursor *vResizeCursor = nullptr;
        GLFWcursor *nwseResizeCursor = nullptr;
        GLFWcursor *neswResizeCursor = nullptr;

        void handleWindowManagement(int button, int action, int mods);
        void handleCursorPos(double xpos, double ypos);
        void updateCursorShape(double xpos, double ypos);
        ResizeRegion getResizeRegion(double xpos, double ypos);
        void applySnap(const SnapTarget& target);
        SnapTarget calculateSnapTarget(double xpos, double ypos);

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"VulkanContext", &vulkanContext}};
        }

        void updateWindowManagerInfo(const WindowManagerInfo& info) {
            windowManagerInfo = info;
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

        void disposeCursors();
    };
}

#endif
