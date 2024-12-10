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

        void startMapping();

        void stopMapping() const;

        void dispose(VulkanContext &context) override {
        }
    };
}
#endif //COMMANDBUFFERINSTANCE_H
