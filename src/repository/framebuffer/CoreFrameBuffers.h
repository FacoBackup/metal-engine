#ifndef COREREFRAMEBUFFERS_H
#define COREREFRAMEBUFFERS_H
#include "../abstract/AbstractCoreRepository.h"


namespace Metal {
    struct FrameBufferInstance;

    struct CoreFrameBuffers final : AbstractCoreRepository {
        FrameBufferInstance *gBufferFBO = nullptr;
        FrameBufferInstance *auxFBO = nullptr;
        FrameBufferInstance *aoFBO = nullptr;
        FrameBufferInstance *postProcessingFBO = nullptr;

        explicit CoreFrameBuffers(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;
    };
}

#endif
