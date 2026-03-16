#ifndef ABSTRACTPASS_H
#define ABSTRACTPASS_H
#include <vector>
#include <string>
#include <vulkan/vulkan.h>

#include "../../common/IContextMember.h"
#include "../../common/ISync.h"
#include "../../common/IInit.h"
#include "../resource/RuntimeResource.h"

namespace Metal {
    class EngineFrame;
    struct WorldRepository;
    struct MeshInstance;
    class StreamingService;
    struct PipelineInstance;

    class AbstractPass : public IContextMember, public ISync, public IInit {
        bool isComputePass;

    public:
        EngineFrame *frame = nullptr;
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        PipelineInstance *pipelineInstance = nullptr;

        explicit AbstractPass( bool isComputePass);

        void recordPushConstant(const void *data);

        virtual bool requiresRayTracing() {
            return false;
        }

        virtual bool shouldRun() {
            return true;
        }

        void setCommandBuffer(VkCommandBuffer vkCommandBuffer) {
            this->vkCommandBuffer = vkCommandBuffer;
        }

        PipelineInstance *getPipeline() const {
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
