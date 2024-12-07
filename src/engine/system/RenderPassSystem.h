#ifndef RENDERPASSSYSTEM_H
#define RENDERPASSSYSTEM_H
#include "../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    /**
     * Executes 'onSync' for each render pass and manages all the render passes
     */
    class RenderPassSystem final : public AbstractRuntimeComponent {

    public:
        explicit RenderPassSystem(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }
    };
} // Metal

#endif
