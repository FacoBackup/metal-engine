#include "FrameBufferAttachment.h"
#include "../VulkanContext.h"

namespace Metal {
    void FrameBufferAttachment::dispose(const VulkanContext &context) const {
        vkDestroySampler(context.device.device, vkImageSampler, nullptr);
        vkDestroyImage(context.device.device, vkImage, nullptr);
        vkDestroyImageView(context.device.device, vkImageView, nullptr);
        vkFreeMemory(context.device.device, mem, nullptr);

        if (imageDescriptor != nullptr) {
            imageDescriptor->dispose(context);
        }
    }
}
