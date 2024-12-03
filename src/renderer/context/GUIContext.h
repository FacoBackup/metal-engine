#ifndef METAL_ENGINE_GUICONTEXT_H
#define METAL_ENGINE_GUICONTEXT_H

#include "GLFWContext.h"
#include "imgui_impl_glfw.h"
#include "../../ui/common/IPanel.h"

namespace Metal {

    class GUIContext {
    public:

        void build(bool debugMode);

        VulkanContext &getVulkanContext();

        GLFWContext &getWindowContext();

        void shutdown();

        void setupContext();

        bool beginFrame();

        void endFrame();
    private:

        GLFWContext windowContext;
    };

}

#endif