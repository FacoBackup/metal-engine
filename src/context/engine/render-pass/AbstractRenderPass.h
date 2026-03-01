#ifndef ABSTRACTRENDERPASS_H
#define ABSTRACTRENDERPASS_H

#include "../passes/AbstractPass.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct WorldRepository;
    struct MeshInstance;
    class StreamingService;
    struct PipelineInstance;

    class AbstractRenderPass : public AbstractPass {
    public:
        explicit AbstractRenderPass(const std::string &id);

        void recordDrawSimpleInstanced(unsigned int vertexCount, unsigned int instanceCount) const;

        void recordDrawMesh(const MeshInstance *instance, unsigned int instanceCount = 1) const;
    };
}

#endif
