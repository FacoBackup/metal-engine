#ifndef METAL_ENGINE_GLOBALSETTINGSPANEL_H
#define METAL_ENGINE_GLOBALSETTINGSPANEL_H

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class GlobalSettingsPanel final : public AbstractPanel {
        void cameraMode() const;

        void shadingMode();

        int editorMode = 0;
        int shadingModelOption = 0;

    public:
        void onSync() override;
    };
} // Metal

#endif
