#ifndef SSBOINSTANCE_H
#define SSBOINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "BufferType.h"
#include "../../../repository/resource/RuntimeResource.h"
#include "../../../../common/runtime/VulkanContext.h"

namespace Metal {
    struct BufferInstance final : RuntimeResource {
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        VkDeviceSize size = 0;
        BufferType type{};

        void dispose(VulkanContext &context) override {
            if (buffer != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE) {
                vmaDestroyBuffer(context.allocator, buffer, allocation);
                buffer = VK_NULL_HANDLE;
                allocation = VK_NULL_HANDLE;
            }
        }

        void updateData(VulkanContext &context, const void *data, VkDeviceSize dataSize) {
            if (dataSize > size) {
                throw std::runtime_error("Data size exceeds buffer size!");
            }

            void *mappedData;
            vmaMapMemory(context.allocator, allocation, &mappedData);
            std::memcpy(mappedData, data, static_cast<size_t>(dataSize));
            vmaUnmapMemory(context.allocator, allocation);
        }
    };
} // Metal

#endif //SSBOINSTANCE_H
