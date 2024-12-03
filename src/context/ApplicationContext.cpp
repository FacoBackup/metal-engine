#include "ApplicationContext.h"
#include "../ui/EditorPanel.h"

namespace Metal {
    ApplicationContext::ApplicationContext() {
        auto *editorPanel = new EditorPanel;
        panel = editorPanel;
        guiContext.setRoot(panel);
        editorPanel->setContext(this);
    }

    Engine &ApplicationContext::getEngine() {
        return engine;
    }

    GUIContext &ApplicationContext::getGuiContext() {
        return guiContext;
    }
}