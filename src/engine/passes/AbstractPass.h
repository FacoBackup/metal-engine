#ifndef ABSTRACTPASS_H
#define ABSTRACTPASS_H
#include <vector>
#include <string>
#include <vulkan/vulkan.h>

#include "../../common/IContextMember.h"
#include "../../common/IEventMember.h"
#include "../../common/ISync.h"
#include "../../common/IInit.h"
#include "../resource/RuntimeResource.h"

#include "../render-graph/RGResourceHandle.h"
#include "../render-graph/RGPassBuilder.h"
#include "../render-graph/RGResourceManager.h"
#include "../render-graph/RGBarrierBatch.h"

namespace Metal {
    class EngineFrame;
    struct WorldRepository;
    struct MeshInstance;
    class StreamingService;
    struct PipelineInstance;

    class AbstractPass : public IContextMember, public IEventMember, public ISync, public IInit {
        bool isComputePass;
        std::unordered_map<std::string, RGResourceHandle> resourceHandles;
        RGBarrierBatch barrierBatch;

    public:
        EngineFrame *frame = nullptr;
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        PipelineInstance *pipelineInstance = nullptr;

        explicit AbstractPass( bool isComputePass);

        virtual void setup(RGPassBuilder& builder) {}
        virtual void execute(VkCommandBuffer cmd, const RGResourceManager& resources) {}

        void setBarrierBatch(const RGBarrierBatch& batch) {
            barrierBatch = batch;
        }

        const RGBarrierBatch& getBarrierBatch() const {
            return barrierBatch;
        }

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

        void addResourceHandle(const std::string &id, RGResourceHandle handle) {
            resourceHandles[id] = handle;
        }

        [[nodiscard]] RGResourceHandle getResourceHandle(const std::string &id) const {
            auto it = resourceHandles.find(id);
            if (it != resourceHandles.end()) {
                return it->second;
            }
            return RGResourceHandle();
        }
    };
} // Metal

#endif //ABSTRACTPASS_H
