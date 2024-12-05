#ifndef METAL_ENGINE_DOCKREPOSITORY_H
#define METAL_ENGINE_DOCKREPOSITORY_H

#include "../../../common/runtime/AbstractRuntimeComponent.h"
#include "DockDTO.h"

namespace Metal {
    class DockSpacePanel;

    struct DockRepository final : AbstractRuntimeComponent {
        DockDTO center{&DockSpace::VIEWPORT};
        std::vector<DockDTO *> bottom;
        std::vector<DockDTO *> left;
        std::vector<DockDTO *> right;
        DockDTO *dockToRemove;
        DockSpacePanel *dockPanelToRemove;
        bool isInitialized = false;

        explicit DockRepository(ApplicationContext &context);
    };
} // Metal

#endif
