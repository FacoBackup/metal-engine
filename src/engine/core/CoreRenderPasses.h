#ifndef CORERENDERPASSES_H
#define CORERENDERPASSES_H
#include "AbstractCoreRepository.h"

namespace Metal {
    struct RenderPassInstance;

    struct CoreRenderPasses final : AbstractCoreRepository {
        RenderPassInstance *gBufferRenderPass = nullptr;
        RenderPassInstance *auxRenderPass = nullptr;

        explicit CoreRenderPasses(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;
    };
}

#endif
