#include "BufferService.h"

#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/BufferInstance.h"

namespace Metal {
    BufferInstance *BufferService::createBuffer(VkDeviceSize bufferSize,
                                                VkBufferUsageFlags usageFlags,
                                                VkMemoryPropertyFlags memoryPropertyFlags) const {
        auto *buffer = new BufferInstance{bufferSize};
        createVkBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer);
        vkMapMemory(context.getVulkanContext().device.device, buffer->vkDeviceMemory, 0, bufferSize, 0, &buffer->mapped);
        registerResource(buffer);
        return buffer;
    }

    void BufferService::createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                       BufferInstance *buffer) const {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VulkanUtils::CheckVKResult(vkCreateBuffer(context.getVulkanContext().device.device, &bufferInfo, nullptr,
                                                  &buffer->vkBuffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(context.getVulkanContext().device.device, buffer->vkBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        VulkanUtils::CheckVKResult(vkAllocateMemory(context.getVulkanContext().device.device, &allocInfo, nullptr,
                                                    &buffer->vkDeviceMemory));
        vkBindBufferMemory(context.getVulkanContext().device.device, buffer->vkBuffer, buffer->vkDeviceMemory, 0);
    }

    uint32_t BufferService::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(context.getVulkanContext().physDevice.physical_device, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
} // Metal
