#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H

#include "../engine/EngineContext.h"
#include "GuiContext.h"
#include "../editor/common/IPanel.h"
#include "../editor/EditorContext.h"

namespace Metal {
    class ApplicationContext {
        EngineContext engineContext{*this};
        EditorContext editorContext{*this};
        GLFWContext glfwContext{*this};
        VulkanContext vulkanContext;
        GuiContext guiContext{*this};
        IPanel &rootPanel;

    public:
        void dispose();

        EngineContext &getEngineContext();

        EditorContext &getEditorContext();

        GLFWContext &getGLFWContext();

        VulkanContext &getVulkanContext();

        void start();

        bool isValidContext() const;

        explicit ApplicationContext(IPanel &root_panel, bool debugMode);
    };
}

#endif
