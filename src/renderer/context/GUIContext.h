#ifndef METAL_ENGINE_GUICONTEXT_H
#define METAL_ENGINE_GUICONTEXT_H

#include "GLFWContext.h"
#include "imgui_impl_glfw.h"
#include "../../ui/IPanel.h"

namespace Metal {

    class GUIContext {
    public:

        void build(bool debugMode);

        VulkanContext &getVulkanContext();

        GLFWContext &getWindowContext();

        void shutdown();

        void setupContext();

        void start();

        void setRoot(IPanel *root);

    private:

        bool beginFrame();

        void endFrame();

        GLFWContext windowContext;
        IPanel *root = nullptr;
    };

}

#endif