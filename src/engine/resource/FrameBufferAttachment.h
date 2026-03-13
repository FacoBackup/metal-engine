#ifndef FRAMEBUFFERATTACHMENT_H
#define FRAMEBUFFERATTACHMENT_H
#include "../dto/DescriptorInstance.h"

namespace Metal {
    struct FrameBufferAttachment final {
        VkImage vkImage = VK_NULL_HANDLE;
        VkDeviceMemory vkImageMemory = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkFormat format = VK_FORMAT_MAX_ENUM;
        DescriptorInstance *imageDescriptor = nullptr;
        bool depth = false;

        void dispose() const;
    };
}

#endif
