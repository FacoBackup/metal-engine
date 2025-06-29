#ifndef COREREFRAMEBUFFERS_H
#define COREREFRAMEBUFFERS_H
#include "../../common/AbstractRuntimeComponent.h"


namespace Metal {
    struct FrameBufferInstance;

    struct CoreFrameBuffers final : AbstractRuntimeComponent {
        FrameBufferInstance *postProcessingFBO = nullptr;

        void onInitialize() override;
    };
}

#endif
