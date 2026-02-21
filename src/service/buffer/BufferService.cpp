#include "BufferService.h"

#include "../../context/ApplicationContext.h"
#include "../../util/VulkanUtils.h"
#include "BufferInstance.h"

namespace Metal {

    std::shared_ptr<BufferInstance> BufferService::createBuffer(VkDeviceSize bufferSize,
                                                                VkBufferUsageFlags usageFlags,
                                                                VkMemoryPropertyFlags memoryPropertyFlags) const {
        std::shared_ptr<BufferInstance> buffer(new BufferInstance{bufferSize});
        createVkBuffer(usageFlags, memoryPropertyFlags, buffer);
        vkMapMemory(CTX.vulkanContext.device.device, buffer->vkDeviceMemory, 0, bufferSize, 0, &buffer->mapped);
        return buffer;
    }

    std::shared_ptr<BufferInstance> BufferService::createBuffer(VkDeviceSize dataSize,
                                                                VkBufferUsageFlags usageFlags,
                                                                const void *bufferData) const {
        std::shared_ptr<BufferInstance> stagingBuffer(new BufferInstance{dataSize});
        std::shared_ptr<BufferInstance> finalBuffer(new BufferInstance{dataSize});

        createVkBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

        vkMapMemory(CTX.vulkanContext.device.device, stagingBuffer->vkDeviceMemory, 0, dataSize, 0, &stagingBuffer->mapped);
        memcpy(stagingBuffer->mapped, bufferData, dataSize);
        vkUnmapMemory(CTX.vulkanContext.device.device, stagingBuffer->vkDeviceMemory);

        createVkBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, finalBuffer);

        copyBuffer(stagingBuffer, finalBuffer);

        stagingBuffer->dispose();
        return finalBuffer;
    }

    unsigned int BufferService::findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties) const {
        for (unsigned int i = 0; i < CTX.vulkanContext.physicalDeviceMemoryProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (CTX.vulkanContext.physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags &
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

        VulkanUtils::CheckVKResult(vkCreateBuffer(CTX.vulkanContext.device.device, &bufferInfo, nullptr,
                                                  &buffer->vkBuffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(CTX.vulkanContext.device.device, buffer->vkBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        VulkanUtils::CheckVKResult(vkAllocateMemory(CTX.vulkanContext.device.device, &allocInfo, nullptr,
                                                    &buffer->vkDeviceMemory));
        vkBindBufferMemory(CTX.vulkanContext.device.device, buffer->vkBuffer, buffer->vkDeviceMemory, 0);
    }

    void BufferService::copyBuffer(const std::shared_ptr<BufferInstance> &srcBuffer,
                                   const std::shared_ptr<BufferInstance> &dstBuffer) const {
        VkCommandBuffer commandBuffer = CTX.vulkanContext.beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = dstBuffer->dataSize;
        vkCmdCopyBuffer(commandBuffer, srcBuffer->vkBuffer, dstBuffer->vkBuffer, 1, &copyRegion);

        CTX.vulkanContext.endSingleTimeCommands(commandBuffer);
    }

    std::shared_ptr<BufferInstance> BufferService::createBuffer(VkDeviceSize bufferSize,
                                                                VkBufferUsageFlags usageFlags,
                                                                VkMemoryPropertyFlags memoryPropertyFlags,
                                                                bool deviceAddress) const {
        std::shared_ptr<BufferInstance> buffer(new BufferInstance{bufferSize});

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = usageFlags;
        if (deviceAddress) {
            bufferInfo.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        }
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VulkanUtils::CheckVKResult(vkCreateBuffer(CTX.vulkanContext.device.device, &bufferInfo, nullptr,
                                                  &buffer->vkBuffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(CTX.vulkanContext.device.device, buffer->vkBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);

        VkMemoryAllocateFlagsInfo allocFlagsInfo{};
        if (deviceAddress) {
            allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
            allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
            allocInfo.pNext = &allocFlagsInfo;
        }

        VulkanUtils::CheckVKResult(vkAllocateMemory(CTX.vulkanContext.device.device, &allocInfo, nullptr,
                                                    &buffer->vkDeviceMemory));
        vkBindBufferMemory(CTX.vulkanContext.device.device, buffer->vkBuffer, buffer->vkDeviceMemory, 0);

        if (memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            vkMapMemory(CTX.vulkanContext.device.device, buffer->vkDeviceMemory, 0, bufferSize, 0, &buffer->mapped);
        }

        return buffer;
    }

    std::shared_ptr<BufferInstance> BufferService::createBuffer(VkDeviceSize dataSize,
                                                                VkBufferUsageFlags usageFlags,
                                                                const void *bufferData,
                                                                bool deviceAddress) const {
        std::shared_ptr<BufferInstance> stagingBuffer(new BufferInstance{dataSize});
        createVkBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

        vkMapMemory(CTX.vulkanContext.device.device, stagingBuffer->vkDeviceMemory, 0, dataSize, 0, &stagingBuffer->mapped);
        memcpy(stagingBuffer->mapped, bufferData, dataSize);
        vkUnmapMemory(CTX.vulkanContext.device.device, stagingBuffer->vkDeviceMemory);

        auto finalBuffer = createBuffer(dataSize,
                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        deviceAddress);

        copyBuffer(stagingBuffer, finalBuffer);
        stagingBuffer->dispose();
        return finalBuffer;
    }
} // Metal
