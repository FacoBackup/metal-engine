#ifndef METAL_ENGINE_VIEWPORTPANEL_H
#define METAL_ENGINE_VIEWPORTPANEL_H

#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class ViewportPanel final : public AbstractDockPanel {
        bool isFirstMovement = false;
        AbstractPanel *headerPanel = nullptr;
        AbstractPanel *gizmoPanel = nullptr;
        AbstractPanel *cameraPanel = nullptr;

    public:
        void onInitialize() override;

        void onSync() override;

        void updateCamera();

        void updateInputs() const;
    };
}

#endif
