#ifndef INSPECTOR_H
#define INSPECTOR_H
#include "../../docks/AbstractDockSpacePanel.h"

namespace Metal {
    class InspectorPanel final : public AbstractDockSpacePanel {
        CameraRepository *cameraRepository = nullptr;

    public:
        void onInitialize() override;

        void onSync() override;
    };
}

#endif
