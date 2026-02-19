#ifndef METAL_ENGINE_DOCKREPOSITORY_H
#define METAL_ENGINE_DOCKREPOSITORY_H

#include "DockDTO.h"
#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct DockRepository final : AbstractRuntimeComponent {
        DockDTO top{&DockSpace::VIEWPORT, 0.17f};
        DockDTO bottom{&DockSpace::FILES, .25f};
        DockDTO rightTop{&DockSpace::HIERARCHY, 0.6f};
        DockDTO rightBottom{&DockSpace::INSPECTOR, 0.4f};
        bool isInitialized = false;
    };
} // Metal

#endif
