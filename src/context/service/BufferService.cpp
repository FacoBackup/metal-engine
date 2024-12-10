#include "BufferService.h"

#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/BufferInstance.h"

namespace Metal {
    BufferInstance *BufferService::createBuffer(VkDeviceSize instanceSize,
                                                const uint32_t instanceCount,
                                                VkBufferUsageFlags usageFlags,
                                                VkMemoryPropertyFlags memoryPropertyFlags) {
        auto *buffer = new BufferInstance(
            context.getVulkanContext().device.device,
            instanceSize,
            instanceCount,
            usageFlags,
            memoryPropertyFlags
        );
        createVkBuffer(buffer->getBufferSize(), usageFlags, memoryPropertyFlags, buffer->getBuffer(),
                       buffer->getDeviceMemory());
        registerResource(buffer);
        return buffer;
    }


    void BufferService::createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                       VkMemoryPropertyFlags properties, VkBuffer &buffer,
                                       VkDeviceMemory &bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VulkanUtils::CheckVKResult(vkCreateBuffer(context.getVulkanContext().device.device, &bufferInfo, nullptr,
                                                  &buffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(context.getVulkanContext().device.device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        VulkanUtils::CheckVKResult(vkAllocateMemory(context.getVulkanContext().device.device, &allocInfo, nullptr,
                                                    &bufferMemory));

        VulkanUtils::CheckVKResult(
            vkBindBufferMemory(context.getVulkanContext().device.device, buffer, bufferMemory, 0));
    }

    uint32_t BufferService::findMemoryType(const uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
        for (uint32_t i = 0; i < context.getVulkanContext().physicalDeviceMemoryProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (context.getVulkanContext().physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
} // Metal
