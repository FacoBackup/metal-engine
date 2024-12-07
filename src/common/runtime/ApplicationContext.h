#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H

#include "../../engine/EngineContext.h"
#include "GUIContext.h"
#include "../../editor/common/IPanel.h"
#include "../../editor/EditorContext.h"

namespace Metal {
    class ApplicationContext {
    public:
        EngineContext &getEngineContext();

        EditorContext &getEditorContext();

        GUIContext &getGuiContext();

        GLFWContext &getGLFWContext();

        VulkanContext &getVulkanContext();

        GUIContext &getGUIContext();

        void start();

        bool isValidContext() const;

        explicit ApplicationContext(IPanel &root_panel);

    private:
        EngineContext engineContext{*this};
        EditorContext editorContext{*this};
        GLFWContext glfwContext{*this};
        VulkanContext vulkanContext;
        GUIContext guiContext{*this};
        IPanel &rootPanel;
    };
}

#endif
