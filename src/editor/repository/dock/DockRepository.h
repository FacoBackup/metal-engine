#ifndef METAL_ENGINE_DOCKREPOSITORY_H
#define METAL_ENGINE_DOCKREPOSITORY_H

#include "DockDTO.h"
#include "../../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class DockSpacePanel;

    struct DockRepository final : AbstractRuntimeComponent {
        DockDTO center{&DockSpace::VIEWPORT};
        std::vector<DockDTO *> bottom;
        std::vector<DockDTO *> left;
        std::vector<DockDTO *> right;
        DockDTO *dockToRemove = nullptr;
        DockSpacePanel *dockPanelToRemove = nullptr;
        bool isInitialized = false;

        explicit DockRepository(ApplicationContext &context);
    };
} // Metal

#endif
