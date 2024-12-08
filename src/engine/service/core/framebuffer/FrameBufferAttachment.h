#ifndef FRAMEBUFFERATTACHMENT_H
#define FRAMEBUFFERATTACHMENT_H
#include "../../../repository/resource/RuntimeResource.h"

namespace Metal {
    struct FrameBufferAttachment final : RuntimeResource {
        VkImage vkImage = VK_NULL_HANDLE;
        VkDeviceMemory mem = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkFormat format = VK_FORMAT_MAX_ENUM;
        bool depth = false;
        const char *name = nullptr;

        void dispose(VulkanContext &context) override {
            vkDestroyImage(context.device.device, vkImage, nullptr);
            vkDestroyImageView(context.device.device, vkImageView, nullptr);
            vkFreeMemory(context.device.device, mem, nullptr);
        }
    };
}

#endif
