#include "CommandPoolService.h"

#include "CommandBufferInstance.h"
#include "../../../../common/runtime/ApplicationContext.h"
#include "../../../../common/util/VulkanUtils.h"
#include "../pipeline/PipelineInstance.h"

namespace Metal {
    CommandPoolService::CommandPoolService(ApplicationContext &context)
        : AbstractResourceService(context), poolRepository(context.getEngineContext().poolRepository) {
    }

    void CommandPoolService::createCommandPool() const {
        VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
        cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCreateInfo.pNext = nullptr;
        cmdPoolCreateInfo.flags = 0;
        cmdPoolCreateInfo.queueFamilyIndex = vulkanContext.queueFamily;
        VulkanUtils::CheckVKResult(vkCreateCommandPool(vulkanContext.device.device, &cmdPoolCreateInfo,
                                                       nullptr,
                                                       &poolRepository.commandPool));
    }

    void CommandPoolService::createDescriptorPool() const {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 1; // Adjust this based on your needs

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.flags = 0;
        poolInfo.maxSets = 1; // NUM OF IMAGES
        VulkanUtils::CheckVKResult(vkCreateDescriptorPool(vulkanContext.device.device, &poolInfo,
                                                          nullptr, &poolRepository.descriptorPool));
    }

    void CommandPoolService::onInitialize() {
        createCommandPool();
        createDescriptorPool();
    }

    /**
     * Commands still need to be recorded
     */
    CommandBufferInstance *CommandPoolService::createCommandBuffer(PipelineInstance *pipeline) const {
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

    VkDescriptorSet CommandPoolService::createDescriptorSet() {
        return nullptr;
    }
} // Metal
