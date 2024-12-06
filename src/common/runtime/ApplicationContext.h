#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H

#define ENGINE_NAME "Metal Engine"

#include "../../engine/EngineContext.h"
#include "../../renderer/context/GUIContext.h"
#include "../../editor/common/IPanel.h"
#include "../../editor/EditorContext.h"

namespace Metal {
    class ApplicationContext {
    public:
        explicit ApplicationContext();

        EngineContext &getEngineContext();

        EditorContext &getEditorContext();

        GUIContext &getGuiContext();

        GLFWContext &getGLFWContext();

        VulkanContext &getVulkanContext();

        GUIContext &getGUIContext();

        void start();

        bool isValidContext() const;

    private:
        EngineContext engineContext{*this};
        EditorContext editorContext{*this};
        GLFWContext glfwContext{*this};
        VulkanContext vulkanContext;
        GUIContext guiContext{*this};
        IPanel *rootPanel = nullptr;
    };
}

#endif
