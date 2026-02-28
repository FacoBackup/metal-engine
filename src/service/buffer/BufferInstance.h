#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include <vulkan/vulkan.h>

#include "../../repository/abstract/RuntimeResource.h"

namespace Metal {
    struct BufferInstance final : RuntimeResource {
        void *mapped = nullptr;
        const VkDeviceSize dataSize;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

        explicit BufferInstance(const std::string &id, const VkDeviceSize dataSize)
            : RuntimeResource(id), dataSize(dataSize) {
        }

        void update(void *newData) const;

        ResourceType resourceType() override {
            return BUFFER;
        }
    };
} // Metal

#endif //VULKANBUFFER_H
