#ifndef METAL_ENGINE_EDITORHEADERPANEL_H
#define METAL_ENGINE_EDITORHEADERPANEL_H

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class EditorHeaderPanel final : public AbstractPanel {
        AbstractPanel *globalSettings = nullptr;
        AbstractPanel *projectInfo = nullptr;
        AbstractPanel *asyncTask = nullptr;

        void renderFileTab();

        static void framerate();

        void hotKeys();

    public:
        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif
