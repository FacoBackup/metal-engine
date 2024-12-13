#ifndef METAL_ENGINE_VIEWPORTPANEL_H
#define METAL_ENGINE_VIEWPORTPANEL_H

#include "../../common/IPanel.h"
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class ViewportPanel final : public AbstractDockPanel {
        bool isFirstMovement = false;

    public:

        void onInitialize() override;

        void onSync() override;

        void updateCamera();

        void updateInputs() const;
    };
}

#endif
