#include "ApplicationContext.h"

namespace Metal {
    void ApplicationContext::dispose() {
        guiContext.dispose();
        engineContext.dispose();
    }

    EngineContext &ApplicationContext::getEngineContext() {
        return engineContext;
    }

    GLFWContext &ApplicationContext::getGLFWContext() {
        return glfwContext;
    }

    VulkanContext &ApplicationContext::getVulkanContext() {
        return vulkanContext;
    }

    void ApplicationContext::start() {
        glfwContext.onInitialize();
        if (!glfwContext.isValidContext()) {
            throw std::runtime_error("Could not create window");
        }
        vulkanContext.onInitialize();
        guiContext.onInitialize();
        engineContext.onInitialize();
        rootPanel.onInitialize();


        GLFWwindow *window = glfwContext.getWindow();
        while (!glfwWindowShouldClose(window)) {
            if (guiContext.beginFrame()) {
                continue;
            }
            rootPanel.onSync();
            engineContext.onSync();
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

    ApplicationContext::ApplicationContext(IPanel &root_panel, bool debugMode) : vulkanContext(*this, debugMode),
                                                                                 rootPanel(root_panel) {
    }
}
