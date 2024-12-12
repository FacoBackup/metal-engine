#include "BufferService.h"

#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/BufferInstance.h"

namespace Metal {
    BufferInstance *BufferService::createBuffer(VkDeviceSize bufferSize,
                                                VkBufferUsageFlags usageFlags,
                                                VkMemoryPropertyFlags memoryPropertyFlags) const {
        auto *buffer = new BufferInstance{bufferSize};
        createVkBuffer(usageFlags, memoryPropertyFlags, buffer);
        vkMapMemory(vulkanContext.device.device, buffer->vkDeviceMemory, 0, bufferSize, 0, &buffer->mapped);
        registerResource(buffer);
        return buffer;
    }

    void BufferService::createVkBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                       BufferInstance *buffer) const {
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

    uint32_t BufferService::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
        for (uint32_t i = 0; i < vulkanContext.physicalDeviceMemoryProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (vulkanContext.physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags &
                                            properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    template<class T>
    BufferInstance *BufferService::createBuffer(std::vector<T> bufferData) {
        VkDeviceSize bufferSize = sizeof(bufferData[0]) * bufferData.size();
        auto *stagingBuffer = new BufferInstance{bufferSize};
        auto *finalBuffer = new BufferInstance{bufferSize};
        registerResource(finalBuffer);

        createVkBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

        void *data;
        vkMapMemory(vulkanContext.device.device, stagingBuffer->vkDeviceMemory, 0, bufferSize, 0, &data);
        memcpy(data, bufferData.data(), bufferSize);
        vkUnmapMemory(vulkanContext.device.device, stagingBuffer->vkDeviceMemory);

        createVkBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, finalBuffer);

        copyBuffer(stagingBuffer, finalBuffer);

        stagingBuffer->dispose(vulkanContext);
        return finalBuffer;
    }

    void BufferService::copyBuffer(const BufferInstance *srcBuffer, const BufferInstance *dstBuffer) const {
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
