#ifndef METAL_ENGINE_PROJECTINFOPANEL_H
#define METAL_ENGINE_PROJECTINFOPANEL_H

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class ProjectInfoPanel final : public AbstractPanel {
        char projectName[128] = "";
    public:
        void onSync() override;
    };
}

#endif
