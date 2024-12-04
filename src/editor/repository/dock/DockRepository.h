#ifndef METAL_ENGINE_DOCKREPOSITORY_H
#define METAL_ENGINE_DOCKREPOSITORY_H

#include "../../../common/runtime/AbstractRuntimeComponent.h"

namespace Metal {

    class DockRepository : public AbstractRuntimeComponent {
    public:
        explicit DockRepository(ApplicationContext &context);
    };

} // Metal

#endif