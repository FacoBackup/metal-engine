#ifndef ABSTRACTRENDERPASS_H
#define ABSTRACTRENDERPASS_H

#include "../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    struct PipelineInstance;
}

namespace Metal {
    struct FrameData;
    struct CoreFrameBuffers;
    struct CorePipelines;
    struct CoreBuffers;

    class AbstractRenderPass : public AbstractRuntimeComponent {
    protected:
        CoreBuffers &buffers;
        CorePipelines &pipelines;
        CoreFrameBuffers &frameBuffers;

    public:
        explicit AbstractRenderPass(ApplicationContext &context);
    };
}

#endif
