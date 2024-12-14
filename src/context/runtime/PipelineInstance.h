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
        FrameBufferInstance *frameBuffer = nullptr;
        std::vector<DescriptorInstance *> descriptorSets{};
        uint32_t pushConstantsSize = 0;
        VkRenderPassBeginInfo renderPassInfo{};
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;

        void startRecording();

        void stopRecording() const;

        void recordPushConstant(void *data) const;

        void recordDrawSimpleInstanced(uint32_t vertexCount, uint32_t instanceCount) const;

        void dispose(VulkanContext &context) override;

        void bindMesh(const MeshInstance *instance, uint32_t instanceCount = 1) const;
    };
}
#endif //PIPELINEINSTANCE_H
