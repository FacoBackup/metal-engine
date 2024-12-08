#ifndef COMMANDBUFFERINSTANCE_H
#define COMMANDBUFFERINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../../repository/resource/RuntimeResource.h"

namespace Metal {
    class PipelineInstance;

    struct CommandBufferInstance final : RuntimeResource {
        VkCommandBuffer buffer = VK_NULL_HANDLE;

        VkRenderPassBeginInfo startMapping(const PipelineInstance *pipeline) const;

        void stopMapping() const;

        void dispose(VulkanContext &context) override {
        }
    };
}
#endif //COMMANDBUFFERINSTANCE_H
