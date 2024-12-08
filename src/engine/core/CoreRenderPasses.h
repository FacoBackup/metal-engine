#ifndef CORERENDERPASSES_H
#define CORERENDERPASSES_H
#include "AbstractCoreRepository.h"

namespace Metal {
    struct FrameBufferInstance;

    struct CoreRenderPasses final : AbstractCoreRepository {
        FrameBufferInstance *gBufferRenderPass = nullptr;
        FrameBufferInstance *auxRenderPass = nullptr;

        explicit CoreRenderPasses(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;
    };
}

#endif
