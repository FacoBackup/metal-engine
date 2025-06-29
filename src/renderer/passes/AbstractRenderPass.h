#ifndef ABSTRACTRENDERPASS_H
#define ABSTRACTRENDERPASS_H

#include "AbstractPass.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct WorldRepository;
    class StreamingService;
    struct CoreFrameBuffers;
    struct PipelineInstance;
    struct CoreBuffers;

    class AbstractRenderPass : public AbstractPass {
    public:
        explicit AbstractRenderPass();

        void recordDrawSimpleInstanced(unsigned int  vertexCount, unsigned int  instanceCount) const;
    };
}

#endif
