#ifndef METAL_ENGINE_VIEWPORTPANEL_H
#define METAL_ENGINE_VIEWPORTPANEL_H

#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class ViewportPanel final : public AbstractDockPanel {
        bool isFirstMovement = false;
        AbstractPanel *headerPanel = nullptr;
        AbstractPanel *gizmoPanel = nullptr;
        AbstractPanel *cameraPanel = nullptr;
        AbstractPanel *engineFramePanel = nullptr;

        void handleViewportPicking(const ImVec2 &imageMin, const ImVec2 &imageMax) const;

    public:
        void onInitialize() override;

        void onSync() override;

        void updateCamera();

        void updateInputs() const;
    };
}

#endif
