#ifndef PIPELINEINSTANCE_H
#define PIPELINEINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../repository/abstract/RuntimeResource.h"

namespace Metal {
    struct FrameBufferInstance;
    struct ShaderModule;
    struct DescriptorInstance;

    struct PipelineInstance final : RuntimeResource {
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> descriptorSets{};
        bool isCompute = false;
        uint32_t pushConstantsSize = 0;

        void dispose(VulkanContext &context) override;

        ResourceType resourceType() override {
            return PIPELINE;
        }
    };
}
#endif //PIPELINEINSTANCE_H
