#ifndef METAL_ENGINE_GUICONTEXT_H
#define METAL_ENGINE_GUICONTEXT_H

#include "GLFWContext.h"
#include "imgui_impl_glfw.h"

namespace Metal {

    class GUIContext {
    public:
        void build(bool debugMode);

        VulkanContext &getVulkanContext();

        GLFWContext &getWindowContext();

        void shutdown();

        bool beginFrame();

        void endFrame();

        void setupContext();

    private:
        GLFWContext windowContext;
    };

}

#endif