#ifndef ABSTRACTPASS_H
#define ABSTRACTPASS_H
#include <vulkan/vulkan.h>
#include "../../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct WorldRepository;
    struct MeshInstance;
    class StreamingRepository;
    struct CoreFrameBuffers;
    struct CorePipelines;
    struct PipelineInstance;
    struct CoreBuffers;

    class AbstractPass : public AbstractRuntimeComponent {
        bool isComputePass;
    public:
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        WorldRepository &worldRepository;
        StreamingRepository &streamingRepository;

        explicit AbstractPass(ApplicationContext &context, bool isComputePass);

        void recordPushConstant(const void *data);

        virtual bool shouldRun() {
            return true;
        }

        void setCommandBuffer(VkCommandBuffer vkCommandBuffer) {
            this->vkCommandBuffer = vkCommandBuffer;
        }

        virtual PipelineInstance *getPipeline() {
            return nullptr;
        }
    };
} // Metal

#endif //ABSTRACTPASS_H
