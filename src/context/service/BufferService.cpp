#include "BufferService.h"

#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/BufferInstance.h"

namespace Metal {
    BufferService::BufferService(ApplicationContext &context) : AbstractRuntimeComponent(context),
                                                                vulkanContext(context.getVulkanContext()) {
    }

    std::shared_ptr<BufferInstance> BufferService::createBuffer(VkDeviceSize bufferSize,
                                                                VkBufferUsageFlags usageFlags,
                                                                VkMemoryPropertyFlags memoryPropertyFlags) const {
        std::shared_ptr<BufferInstance> buffer(new BufferInstance{bufferSize});
        createVkBuffer(usageFlags, memoryPropertyFlags, buffer);
        vkMapMemory(vulkanContext.device.device, buffer->vkDeviceMemory, 0, bufferSize, 0, &buffer->mapped);
        return buffer;
    }

    std::shared_ptr<BufferInstance> BufferService::createBuffer(VkDeviceSize dataSize,
                                                                VkBufferUsageFlags usageFlags,
                                                                const void *bufferData) const {
        std::shared_ptr<BufferInstance> stagingBuffer(new BufferInstance{dataSize});
        std::shared_ptr<BufferInstance> finalBuffer(new BufferInstance{dataSize});

        createVkBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

        vkMapMemory(vulkanContext.device.device, stagingBuffer->vkDeviceMemory, 0, dataSize, 0, &stagingBuffer->mapped);
        memcpy(stagingBuffer->mapped, bufferData, dataSize);
        vkUnmapMemory(vulkanContext.device.device, stagingBuffer->vkDeviceMemory);

        createVkBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, finalBuffer);

        copyBuffer(stagingBuffer, finalBuffer);

        stagingBuffer->dispose(vulkanContext);
        return finalBuffer;
    }

    uint32_t BufferService::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
        for (uint32_t i = 0; i < vulkanContext.physicalDeviceMemoryProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (vulkanContext.physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags &
                                            properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void BufferService::createVkBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                       const std::shared_ptr<BufferInstance> &buffer) const {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = buffer->dataSize;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VulkanUtils::CheckVKResult(vkCreateBuffer(vulkanContext.device.device, &bufferInfo, nullptr,
                                                  &buffer->vkBuffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(vulkanContext.device.device, buffer->vkBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        VulkanUtils::CheckVKResult(vkAllocateMemory(vulkanContext.device.device, &allocInfo, nullptr,
                                                    &buffer->vkDeviceMemory));
        vkBindBufferMemory(vulkanContext.device.device, buffer->vkBuffer, buffer->vkDeviceMemory, 0);
    }

    void BufferService::copyBuffer(const std::shared_ptr<BufferInstance> &srcBuffer,
                                   const std::shared_ptr<BufferInstance> &dstBuffer) const {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vulkanContext.commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(vulkanContext.device.device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = dstBuffer->dataSize;
        vkCmdCopyBuffer(commandBuffer, srcBuffer->vkBuffer, dstBuffer->vkBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(vulkanContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(vulkanContext.graphicsQueue);

        vkFreeCommandBuffers(vulkanContext.device.device, vulkanContext.commandPool, 1, &commandBuffer);
    }
} // Metal
