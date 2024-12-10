#ifndef COMMANDBUFFERINSTANCE_H
#define COMMANDBUFFERINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "RuntimeResource.h"

namespace Metal {
    class PipelineInstance;

    struct CommandBufferInstance final : RuntimeResource {
        VkRenderPassBeginInfo renderPassInfo{};
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        PipelineInstance *pipeline = nullptr;

        void startRecording();

        void stopRecording() const;

        void dispose(VulkanContext &context) override {
        }

        void recordPushConstant(void *data) const;

        void recordDrawSimpleInstanced(uint32_t vertexCount, uint32_t instanceCount) const;
    };
}
#endif //COMMANDBUFFERINSTANCE_H
