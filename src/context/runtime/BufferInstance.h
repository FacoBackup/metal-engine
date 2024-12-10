#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include <vulkan/vulkan.h>

#include "RuntimeResource.h"

// THANKS TO https://github.com/blurrypiano/littleVulkanEngine/blob/master/littleVulkanEngine/tutorial19
namespace Metal {
    class BufferInstance final : public RuntimeResource {
        static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        VkDevice vkDevice = VK_NULL_HANDLE;
        void *mapped = nullptr;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

        VkDeviceSize vkBufferSize;
        uint32_t instanceCount;
        VkDeviceSize vkInstanceSize;
        VkDeviceSize vkAlignmentSize;
        VkBufferUsageFlags vkUsageFlags;
        VkMemoryPropertyFlags vkMemoryPropertyFlags;

    public:
        BufferInstance(
            VkDevice device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment = 1);

        void dispose(Metal::VulkanContext &context) override;

        BufferInstance(const BufferInstance &) = delete;

        BufferInstance &operator=(const BufferInstance &) = delete;

        void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void unmap();

        void writeToBuffer(const void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;

        void flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;

        void invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;

        void writeToIndex(void *data, int index);

        void flushIndex(int index) const;

        void invalidateIndex(int index) const;

        [[nodiscard]] VkBuffer &getBuffer() { return vkBuffer; }

        [[nodiscard]] VkDeviceMemory &getDeviceMemory() { return vkDeviceMemory; }

        [[nodiscard]] void *getMappedMemory() const { return mapped; }

        [[nodiscard]] uint32_t getInstanceCount() const { return instanceCount; }

        [[nodiscard]] VkDeviceSize getInstanceSize() const { return vkInstanceSize; }

        [[nodiscard]] VkDeviceSize getAlignmentSize() const { return vkInstanceSize; }

        [[nodiscard]] VkBufferUsageFlags getUsageFlags() const { return vkUsageFlags; }

        [[nodiscard]] VkMemoryPropertyFlags getMemoryPropertyFlags() const { return vkMemoryPropertyFlags; }

        [[nodiscard]] VkDeviceSize getBufferSize() const { return vkBufferSize; }
    };
} // Metal

#endif //VULKANBUFFER_H
