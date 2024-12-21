#ifndef CORERENDERPASSES_H
#define CORERENDERPASSES_H
#include "AbstractCoreRepository.h"

namespace Metal {
    class RenderPass;

    struct CoreRenderPasses final : AbstractCoreRepository {
        explicit CoreRenderPasses(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;

        RenderPass *gBufferPass = nullptr;
        RenderPass *fullScreenPass = nullptr;
        RenderPass *postProcessingPass = nullptr;
    };
} // Metal

#endif //CORERENDERPASSES_H
