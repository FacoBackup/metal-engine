#include "ApplicationContext.h"
#include "../../ui/EditorPanel.h"

namespace Metal {
    ApplicationContext::ApplicationContext() {
        auto *editorPanel = new EditorPanel;
        panel = editorPanel;
        editorPanel->setContext(this);
        guiContext.build(true);
    }

    Engine &ApplicationContext::getEngine() {
        return engine;
    }

    GUIContext &ApplicationContext::getGuiContext() {
        return guiContext;
    }

    void ApplicationContext::start() {
        if (panel != nullptr) {
            engine.onInitialize();
            panel->onInitialize();

            GLFWwindow *window = guiContext.getWindowContext().getWindow();
            while (!glfwWindowShouldClose(window)) {
                if (guiContext.beginFrame()) {
                    continue;
                }
                engine.onSync();
                panel->onSync();
                guiContext.endFrame();
            }
            guiContext.shutdown();
        }
    }

    bool ApplicationContext::isValidContext() {
        return guiContext.getWindowContext().isValidContext();
    }
}