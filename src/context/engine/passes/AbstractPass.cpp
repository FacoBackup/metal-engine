#include "AbstractPass.h"
#include "../frame-builder/EngineFrame.h"
#include "../../../context/ApplicationContext.h"
#include "../../../service/pipeline/PipelineInstance.h"

namespace Metal {
    AbstractPass::AbstractPass(const std::string &id, bool isComputePass) : RuntimeResource(id),
                                                                            isComputePass(isComputePass) {
    }

    AbstractPass::~AbstractPass() {
        if (pipelineInstance != nullptr) {
            CTX.pipelineService.dispose(pipelineInstance->getId());
        }
    }

    void AbstractPass::recordPushConstant(const void *data) {
        VkShaderStageFlags stageFlags = 0;
        if (getPipeline()->isRayTracing) {
            stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR |
                         VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR |
                         VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
        } else if (isComputePass) {
            stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        } else {
            stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        vkCmdPushConstants(
            vkCommandBuffer,
            getPipeline()->vkPipelineLayout,
            stageFlags,
            0,
            getPipeline()->pushConstantsSize,
            data);
    }

    void AbstractPass::bindStaticDescriptorSets() {
        if (getPipeline()->descriptor != nullptr) {
            bindSingleDescriptorSet(0, getPipeline()->descriptor->vkDescriptorSet);
        }
    }

    void AbstractPass::bindSingleDescriptorSet(const unsigned int descriptorSetIndex,
                                               const VkDescriptorSet &descriptorSet) {
        vkCmdBindDescriptorSets(
            vkCommandBuffer,
            getBindingPoint(),
            getPipeline()->vkPipelineLayout,
            descriptorSetIndex,
            1,
            &descriptorSet,
            0,
            nullptr
        );
    }

    void AbstractPass::bindDescriptorSets(const std::vector<VkDescriptorSet> &descriptors) {
        if (!descriptors.empty()) {
            vkCmdBindDescriptorSets(vkCommandBuffer,
                                    getBindingPoint(),
                                    getPipeline()->vkPipelineLayout,
                                    0,
                                    descriptors.size(),
                                    descriptors.data(),
                                    0,
                                    nullptr);
        }
    }

    VkPipelineBindPoint AbstractPass::getBindingPoint() const {
        if (pipelineInstance != nullptr && pipelineInstance->isRayTracing) {
            return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
        }
        if (isComputePass) {
            return VK_PIPELINE_BIND_POINT_COMPUTE;
        }
        return VK_PIPELINE_BIND_POINT_GRAPHICS;
    }

    std::string AbstractPass::getScopedResourceId(const std::string &id) const {
        if (frame != nullptr) {
            return frame->getId() + "_" + id;
        }
        return id;
    }
}
