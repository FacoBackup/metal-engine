#ifndef ABSTRACTRENDERPASS_H
#define ABSTRACTRENDERPASS_H

#include "../passes/AbstractPass.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct WorldRepository;
    class StreamingRepository;
    struct CoreFrameBuffers;
    struct PipelineInstance;
    struct CoreBuffers;

    class AbstractRenderPass : public AbstractPass {
    public:
        explicit AbstractRenderPass(ApplicationContext &context);

        void recordDrawSimpleInstanced(uint32_t vertexCount, uint32_t instanceCount) const;
    };
}

#endif
