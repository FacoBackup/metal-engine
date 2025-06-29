#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include <vulkan/vulkan.h>

namespace Metal {
    struct BufferInstance final {
        void *mapped = nullptr;
        const VkDeviceSize dataSize;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

        explicit BufferInstance(const VkDeviceSize dataSize): dataSize(dataSize) {
        }

        void dispose() const;

        void update(void *newData) const;

        void update(void *newData, VkDeviceSize size) const;
    };
} // Metal

#endif //VULKANBUFFER_H
