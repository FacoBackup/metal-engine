#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H

#include "../../engine/EngineContext.h"
#include "../../renderer/context/GUIContext.h"
#include "../../editor/common/IPanel.h"
#include "../../editor/service/DockService.h"
#include "../../editor/EditorContext.h"

namespace Metal {
    class ApplicationContext {
    public:
        explicit ApplicationContext();

        EngineContext &getEngineContext();

        EditorContext &getEditorContext();

        GUIContext &getGuiContext();

        void start();

        bool isValidContext();

    private:
        EngineContext engineContext{*this};
        EditorContext editorContext{*this};
        GUIContext guiContext;
        IPanel *rootPanel = nullptr;
    };

}

#endif