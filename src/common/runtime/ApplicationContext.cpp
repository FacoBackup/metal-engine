#include "ApplicationContext.h"

namespace Metal {
    ApplicationContext::ApplicationContext(IPanel &r) : rootPanel(r) {
        guiContext.build(true);
    }

    void ApplicationContext::dispose() {
        guiContext.dispose();
        engineContext.dispose();
    }

    EngineContext &ApplicationContext::getEngineContext() {
        return engineContext;
    }

    GUIContext &ApplicationContext::getGuiContext() {
        return guiContext;
    }

    GLFWContext &ApplicationContext::getGLFWContext() {
        return glfwContext;
    }

    VulkanContext &ApplicationContext::getVulkanContext() {
        return vulkanContext;
    }

    GUIContext &ApplicationContext::getGUIContext() {
        return guiContext;
    }

    void ApplicationContext::start() {
        engineContext.onInitialize();
        rootPanel.onInitialize();

        GLFWwindow *window = glfwContext.getWindow();
        while (!glfwWindowShouldClose(window)) {
            if (guiContext.beginFrame()) {
                continue;
            }
            engineContext.onSync();
            rootPanel.onSync();
            guiContext.endFrame();
        }
        guiContext.dispose();
    }

    bool ApplicationContext::isValidContext() const {
        return glfwContext.isValidContext();
    }

    EditorContext &ApplicationContext::getEditorContext() {
        return editorContext;
    }
}
