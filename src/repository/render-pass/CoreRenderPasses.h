#ifndef CORERENDERPASSES_H
#define CORERENDERPASSES_H
#include "../abstract/AbstractCoreRepository.h"


namespace Metal {
    class CommandBufferRecorder;

    struct CoreRenderPasses final : AbstractCoreRepository {
        explicit CoreRenderPasses(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;

        CommandBufferRecorder *gBufferPass = nullptr;
        CommandBufferRecorder *fullScreenPass = nullptr;
        CommandBufferRecorder *postProcessingPass = nullptr;
        CommandBufferRecorder *aoPass = nullptr;
    };
} // Metal

#endif //CORERENDERPASSES_H
