#include "ApplicationContext.h"
#include "../../editor/EditorPanel.h"

namespace Metal {
    ApplicationContext::ApplicationContext() {
        auto *editorPanel = new EditorPanel;
        rootPanel = editorPanel;
        editorPanel->setContext(this);
        guiContext.build(true);
    }

    EngineContext &ApplicationContext::getEngineContext() {
        return engineContext;
    }

    GUIContext &ApplicationContext::getGuiContext() {
        return guiContext;
    }

    GLFWContext & ApplicationContext::getGLFWContext() {
        return glfwContext;
    }

    VulkanContext & ApplicationContext::getVulkanContext() {
        return vulkanContext;
    }

    GUIContext & ApplicationContext::getGUIContext() {
        return guiContext;
    }

    void ApplicationContext::start() {
        if (rootPanel != nullptr) {
            engineContext.onInitialize();
            rootPanel->onInitialize();

            GLFWwindow *window = glfwContext.getWindow();
            while (!glfwWindowShouldClose(window)) {
                if (guiContext.beginFrame()) {
                    continue;
                }
                engineContext.onSync();
                rootPanel->onSync();
                guiContext.endFrame();
            }
            guiContext.shutdown();
        }
    }

    bool ApplicationContext::isValidContext() const {
        return glfwContext.isValidContext();
    }

    EditorContext &ApplicationContext::getEditorContext() {
        return editorContext;
    }
}