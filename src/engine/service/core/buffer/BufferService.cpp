#include "BufferService.h"

#include "BufferInstance.h"

namespace Metal {
    BufferInstance *BufferService::createBuffer(const BufferType type, VkDeviceSize bufferSize, VkBufferUsageFlags usage,
                                                VmaMemoryUsage memoryUsage) const {
        auto *buffer = new BufferInstance;
        buffer->size = bufferSize;
        buffer->type = type;
        // Define the buffer creation info
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Define VMA allocation info
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;

        // Create the buffer with VMA
        if (vmaCreateBuffer(vulkanContext.allocator, &bufferInfo, &allocInfo, &buffer->buffer, &buffer->allocation,
                            nullptr) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create buffer with VMA!");
        }

        registerResource(buffer);
        return buffer;
    }
} // Metal
