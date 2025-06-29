#include "BufferService.h"

#include "../../MetalContextProvider.h"
#include "../util/VulkanUtils.h"
#include "../data/BufferInstance.h"

namespace Metal {
    std::shared_ptr<BufferInstance> BufferService::createBuffer(VkDeviceSize bufferSize,
                                                                VkBufferUsageFlags usageFlags,
                                                                VkMemoryPropertyFlags memoryPropertyFlags) const {
        std::shared_ptr<BufferInstance> buffer(new BufferInstance{bufferSize});
        createVkBuffer(usageFlags, memoryPropertyFlags, buffer);
        vkMapMemory(SINGLETONS.vulkanContext.device.device, buffer->vkDeviceMemory, 0, bufferSize, 0, &buffer->mapped);
        return buffer;
    }

    std::shared_ptr<BufferInstance> BufferService::createBuffer(VkDeviceSize dataSize,
                                                                VkBufferUsageFlags usageFlags,
                                                                const void *bufferData) const {
        std::shared_ptr<BufferInstance> stagingBuffer(new BufferInstance{dataSize});
        std::shared_ptr<BufferInstance> finalBuffer(new BufferInstance{dataSize});

        createVkBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

        vkMapMemory(SINGLETONS.vulkanContext.device.device, stagingBuffer->vkDeviceMemory, 0, dataSize, 0, &stagingBuffer->mapped);
        memcpy(stagingBuffer->mapped, bufferData, dataSize);
        vkUnmapMemory(SINGLETONS.vulkanContext.device.device, stagingBuffer->vkDeviceMemory);

        createVkBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, finalBuffer);

        copyBuffer(stagingBuffer, finalBuffer);

        stagingBuffer->dispose();
        return finalBuffer;
    }

    unsigned int BufferService::findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties) const {
        for (unsigned int i = 0; i < SINGLETONS.vulkanContext.physicalDeviceMemoryProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (SINGLETONS.vulkanContext.physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags &
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

        VulkanUtils::CheckVKResult(vkCreateBuffer(SINGLETONS.vulkanContext.device.device, &bufferInfo, nullptr,
                                                  &buffer->vkBuffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(SINGLETONS.vulkanContext.device.device, buffer->vkBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        VulkanUtils::CheckVKResult(vkAllocateMemory(SINGLETONS.vulkanContext.device.device, &allocInfo, nullptr,
                                                    &buffer->vkDeviceMemory));
        vkBindBufferMemory(SINGLETONS.vulkanContext.device.device, buffer->vkBuffer, buffer->vkDeviceMemory, 0);
    }

    void BufferService::copyBuffer(const std::shared_ptr<BufferInstance> &srcBuffer,
                                   const std::shared_ptr<BufferInstance> &dstBuffer) const {
        VkCommandBuffer commandBuffer = SINGLETONS.vulkanContext.beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = dstBuffer->dataSize;
        vkCmdCopyBuffer(commandBuffer, srcBuffer->vkBuffer, dstBuffer->vkBuffer, 1, &copyRegion);

        SINGLETONS.vulkanContext.endSingleTimeCommands(commandBuffer);
    }
} // Metal
