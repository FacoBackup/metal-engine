#ifndef METAL_ENGINE_EDITORFOOTERPANEL_H
#define METAL_ENGINE_EDITORFOOTERPANEL_H

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class EditorFooterPanel final : public AbstractPanel {
        void renderShortcuts();
        static void framerate();

    public:
        void onSync() override;
    };
}

#endif
