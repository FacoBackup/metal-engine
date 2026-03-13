#ifndef ABSTRACTPASS_H
#define ABSTRACTPASS_H
#include <vector>
#include <string>
#include <vulkan/vulkan.h>
#include "../../common/AbstractRuntimeComponent.h"
#include "../resource/RuntimeResource.h"

namespace Metal {
    class EngineFrame;
    struct WorldRepository;
    struct MeshInstance;
    class StreamingService;
    struct PipelineInstance;

    class AbstractPass : public AbstractRuntimeComponent, public RuntimeResource {
        bool isComputePass;

    public:
        EngineFrame *frame = nullptr;
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        PipelineInstance *pipelineInstance = nullptr;

        explicit AbstractPass(const std::string &id, bool isComputePass);

        virtual ~AbstractPass() override;

        ResourceType resourceType() override {
            return PASS;
        }

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

        [[nodiscard]] std::string getScopedResourceId(const std::string &id) const;
    };
} // Metal

#endif //ABSTRACTPASS_H
