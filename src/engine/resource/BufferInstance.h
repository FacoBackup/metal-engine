#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include <vulkan/vulkan.h>

#include "RuntimeResource.h"

namespace Metal {
    enum BufferType {
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
        OTHER
    };
    struct BufferInstance final : RuntimeResource {
        void *mapped = nullptr;
        const VkDeviceSize dataSize;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        BufferType bufferType;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

        explicit BufferInstance(const std::string &id, const VkDeviceSize dataSize, BufferType bufferType)
            : RuntimeResource(id), dataSize(dataSize), bufferType(bufferType) {
        }

        BufferType getBufferType();

        void update(void *newData) const;

        ResourceType resourceType() override {
            return BUFFER;
        }
    };
} // Metal

#endif //VULKANBUFFER_H
