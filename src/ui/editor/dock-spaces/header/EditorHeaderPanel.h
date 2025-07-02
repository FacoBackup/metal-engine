#ifndef METAL_ENGINE_EDITORHEADERPANEL_H
#define METAL_ENGINE_EDITORHEADERPANEL_H

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class EditorHeaderPanel final : public AbstractPanel {
        char projectName[128] = "";

        void renderFileTab();

        static void framerate();

    public:
        void onSync() override;
    };
} // Metal

#endif
