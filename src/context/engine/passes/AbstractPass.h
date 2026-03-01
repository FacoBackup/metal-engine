#ifndef ABSTRACTPASS_H
#define ABSTRACTPASS_H
#include <vector>
#include <vulkan/vulkan.h>
#include "../../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct WorldRepository;
    struct MeshInstance;
    class StreamingService;
    struct PipelineInstance;

    class AbstractPass : public AbstractRuntimeComponent {
        bool isComputePass;

    public:
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        WorldRepository &worldRepository;
        StreamingService &streamingRepository;
        PipelineInstance *pipelineInstance = nullptr;

        explicit AbstractPass(bool isComputePass);

        virtual ~AbstractPass() override;

        void recordPushConstant(const void *data);

        virtual bool shouldRun() {
            return true;
        }

        void setCommandBuffer(VkCommandBuffer vkCommandBuffer) {
            this->vkCommandBuffer = vkCommandBuffer;
        }

        PipelineInstance *getPipeline() {
            return pipelineInstance;
        }

        virtual void bindStaticDescriptorSets();

        void bindSingleDescriptorSet(unsigned int descriptorSetIndex, const VkDescriptorSet &descriptorSet);

        void bindDescriptorSets(const std::vector<VkDescriptorSet> &descriptors);

        VkPipelineBindPoint getBindingPoint() const;
    };
} // Metal

#endif //ABSTRACTPASS_H
