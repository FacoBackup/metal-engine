#include "CommandService.h"

#include "../runtime/CommandBufferInstance.h"
#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/PipelineInstance.h"

namespace Metal {
    CommandService::CommandService(ApplicationContext &context)
            : AbstractResourceService(context) {
    }

    /**
     * Commands still need to be recorded
     */
    CommandBufferInstance *CommandService::createCommandBuffer(PipelineInstance *pipeline) const {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = context.getVulkanContext().commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer{};
        VulkanUtils::CheckVKResult(
                vkAllocateCommandBuffers(vulkanContext.device.device, &allocInfo, &commandBuffer));

        auto *buffer = new CommandBufferInstance;
        buffer->vkCommandBuffer = commandBuffer;
        buffer->pipeline = pipeline;
        registerResource(buffer);

        return buffer;
    }
} // Metal
