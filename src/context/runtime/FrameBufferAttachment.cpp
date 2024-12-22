#include "FrameBufferAttachment.h"

#include <iostream>

#include "../VulkanContext.h"

namespace Metal {
    void FrameBufferAttachment::dispose(const VulkanContext &context) const {
        vkDestroyImage(context.device.device, vkImage, nullptr);
        vkDestroyImageView(context.device.device, vkImageView, nullptr);
        vkFreeMemory(context.device.device, vkImageMemory, nullptr);

        if (imageDescriptor != nullptr) {
            imageDescriptor->dispose(context);
        }
    }
}
