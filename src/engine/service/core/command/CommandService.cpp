#include "CommandService.h"

#include "CommandBufferInstance.h"
#include "../../../../common/runtime/ApplicationContext.h"
#include "../../../../common/util/VulkanUtils.h"
#include "../pipeline/PipelineInstance.h"

namespace Metal {
    CommandService::CommandService(ApplicationContext &context)
        : AbstractResourceService(context), poolRepository(context.getEngineContext().poolRepository) {
    }

    void CommandService::onInitialize() {
        VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
        cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCreateInfo.pNext = nullptr;
        cmdPoolCreateInfo.flags = 0;
        cmdPoolCreateInfo.queueFamilyIndex = vulkanContext.queueFamily;
        VulkanUtils::CheckVKResult(vkCreateCommandPool(vulkanContext.device.device, &cmdPoolCreateInfo,
                                                       nullptr,
                                                       &poolRepository.commandPool));

    }

    /**
     * Commands still need to be recorded
     */
    CommandBufferInstance *CommandService::createCommandBuffer(PipelineInstance *pipeline) const {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = poolRepository.commandPool;
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

    VkDescriptorSet CommandService::createDescriptorSet() {
        return nullptr;
    }
} // Metal
