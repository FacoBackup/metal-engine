#ifndef METAL_ENGINE_DOCKREPOSITORY_H
#define METAL_ENGINE_DOCKREPOSITORY_H

#include <vector>
#include "../dto/DockDTO.h"
#include "../../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct DockRepository final : AbstractRuntimeComponent {
        DockDTO center{&DockSpace::VIEWPORT};
        std::vector<DockDTO *> bottom;
        std::vector<DockDTO *> left;
        std::vector<DockDTO *> right;

        bool isInitialized = false;

        explicit DockRepository();
    };
} // Metal

#endif
