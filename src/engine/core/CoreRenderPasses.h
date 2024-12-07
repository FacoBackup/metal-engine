#ifndef COREFRAMEBUFFERS_H
#define COREFRAMEBUFFERS_H
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
