#ifndef METAL_ENGINE_DOCKREPOSITORY_H
#define METAL_ENGINE_DOCKREPOSITORY_H

#include "../data/dock/DockDTO.h"
#include "../../common/serialization-definitions.h"

namespace Metal {
    class DockSpacePanel;

    struct DockRepository final {
        DockDTO center{{DockSpace::VIEWPORT.index}};
        std::vector<DockDTO> bottom;
        std::vector<DockDTO> left;
        std::vector<DockDTO> right;
        bool isInitialized = false;

        explicit DockRepository();

        SAVE_TEMPLATE(
            bottom,
            left,
            right,
            center)
    };
} // Metal

#endif
