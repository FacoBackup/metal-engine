#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include <vulkan/vulkan.h>

#include "RuntimeResource.h"

namespace Metal {
    struct BufferInstance final : RuntimeResource {
        void *mapped = nullptr;
        const VkDeviceSize dataSize;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

        explicit BufferInstance(const VkDeviceSize dataSize): dataSize(dataSize) {
        }

        void dispose(VulkanContext &context) override;

        void update( void *newData) const;
    };
} // Metal

#endif //VULKANBUFFER_H
