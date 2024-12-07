#ifndef VULKANFRAMEDATA_H
#define VULKANFRAMEDATA_H
#include <vulkan/vulkan_core.h>

namespace Metal {
    struct VulkanFrameData {
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        VkFence fence = VK_NULL_HANDLE;
        VkImage backbuffer = VK_NULL_HANDLE;
        VkImageView backbufferView = VK_NULL_HANDLE;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkSemaphore imageAcquiredSemaphore = VK_NULL_HANDLE;
        VkSemaphore renderCompleteSemaphore = VK_NULL_HANDLE;
        uint32_t frameIndex{};
    };
}

#endif
