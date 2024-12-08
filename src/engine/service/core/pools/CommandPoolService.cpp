#include "CommandPoolService.h"

#include "CommandBufferInstance.h"
#include "../../../../common/runtime/ApplicationContext.h"
#include "../../../../common/util/VulkanUtils.h"

namespace Metal {
    CommandPoolService::CommandPoolService(ApplicationContext &context)
        : AbstractResourceService(context), poolRepository(context.getEngineContext().poolRepository) {
    }

    void CommandPoolService::onInitialize() {
        VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
        cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCreateInfo.pNext = nullptr;
        cmdPoolCreateInfo.flags = 0;
        cmdPoolCreateInfo.queueFamilyIndex = vulkanContext.queueFamily;
        VulkanUtils::CheckVKResult(vkCreateCommandPool(vulkanContext.device.device, &cmdPoolCreateInfo,
                                                       vulkanContext.instance.allocation_callbacks,
                                                       &poolRepository.commandPool));


        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.flags = 0;
        descriptorPoolCreateInfo.maxSets = 1; // NUM OF IMAGES
        descriptorPoolCreateInfo.poolSizeCount = 0;
        descriptorPoolCreateInfo.pPoolSizes = nullptr;
        VulkanUtils::CheckVKResult(vkCreateDescriptorPool(vulkanContext.device.device, &descriptorPoolCreateInfo,
                                                          vulkanContext.instance.allocation_callbacks, &poolRepository.descriptorPool));
    }

    /**
     * Commands still need to be recorded
     */
    CommandBufferInstance *CommandPoolService::createCommandBuffer() const {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = poolRepository.commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        std::array<VkCommandBuffer, 1> commandBuffers{};
        VulkanUtils::CheckVKResult(vkAllocateCommandBuffers(vulkanContext.device.device, &allocInfo, commandBuffers.data()));

        auto *buffer = new CommandBufferInstance;
        buffer->buffer = commandBuffers[0];
        registerResource(buffer);
        return buffer;
    }

    VkDescriptorSet CommandPoolService::createDescriptorSet() {
        return nullptr;
    }
} // Metal
