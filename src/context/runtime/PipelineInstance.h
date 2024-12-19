#ifndef PIPELINEINSTANCE_H
#define PIPELINEINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "RuntimeResource.h"

namespace Metal {
    struct FrameBufferInstance;
    struct ShaderModule;
    struct MeshInstance;
    struct DescriptorInstance;

    struct PipelineInstance final : RuntimeResource {
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
        std::vector<DescriptorInstance *> descriptorSets{};
        uint32_t pushConstantsSize = 0;

        void dispose(VulkanContext &context) override;

        ResourceType resourceType() override {
            return PIPELINE;
        }
    };
}
#endif //PIPELINEINSTANCE_H
