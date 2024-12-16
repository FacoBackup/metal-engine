#ifndef ABSTRACTRENDERPASS_H
#define ABSTRACTRENDERPASS_H

#include "../../common/interface/AbstractRuntimeComponent.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct WorldRepository;
    struct MeshInstance;
    struct FrameData;
    class StreamingRepository;
    struct CoreFrameBuffers;
    struct CorePipelines;
    struct PipelineInstance;
    struct CoreBuffers;

    class AbstractRenderPass : public AbstractRuntimeComponent {
    protected:
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        WorldRepository &worldRepository;
        StreamingRepository &streamingRepository;

    public:
        explicit AbstractRenderPass(ApplicationContext &context);

        virtual PipelineInstance *getPipeline() {
            return nullptr;
        }

        void recordPushConstant(const void *data);

        void recordDrawSimpleInstanced(uint32_t vertexCount, uint32_t instanceCount) const;

        void drawMesh(const MeshInstance *instance, uint32_t instanceCount = 1) const;

        virtual bool shouldClear() {
            return true;
        }

        void setCommandBuffer(VkCommandBuffer vkCommandBuffer) {
            this->vkCommandBuffer = vkCommandBuffer;
        }
    };
}

#endif
