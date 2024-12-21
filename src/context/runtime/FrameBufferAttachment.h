#ifndef FRAMEBUFFERATTACHMENT_H
#define FRAMEBUFFERATTACHMENT_H
#include "DescriptorInstance.h"

namespace Metal {
    struct FrameBufferAttachment final {
        VkImage vkImage = VK_NULL_HANDLE;
        VkDeviceMemory mem = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkSampler vkImageSampler = VK_NULL_HANDLE;
        VkFormat format = VK_FORMAT_MAX_ENUM;
        std::unique_ptr<DescriptorInstance> imageDescriptor = nullptr;
        bool depth = false;
        const char *name = nullptr;

        void dispose(const VulkanContext &context) const;
    };
}

#endif
