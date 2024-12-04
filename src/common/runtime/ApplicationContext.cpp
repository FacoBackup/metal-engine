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

    void ApplicationContext::start() {
        if (rootPanel != nullptr) {
            engineContext.onInitialize();
            rootPanel->onInitialize();

            GLFWwindow *window = guiContext.getWindowContext().getWindow();
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

    bool ApplicationContext::isValidContext() {
        return guiContext.getWindowContext().isValidContext();
    }

    EditorContext &ApplicationContext::getEditorContext() {
        return editorContext;
    }
}