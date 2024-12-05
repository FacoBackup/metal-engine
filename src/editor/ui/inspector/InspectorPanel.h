#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class CameraRepository;

    class InspectorPanel final : public AbstractDockPanel {
        CameraRepository *cameraRepository = nullptr;

    public:
        void onInitialize() override;

        void onSync() override;
    };
}

#endif
