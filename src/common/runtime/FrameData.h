#ifndef VULKANFRAMEDATA_H
#define VULKANFRAMEDATA_H
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Metal {
    struct FrameData {
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers{};
        VkCommandBuffer imguiCommandBuffer{};
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
