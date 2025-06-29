#include "FrameBufferAttachment.h"


#include "../VulkanContext.h"
#include "../../MetalContextProvider.h"

namespace Metal {
    void FrameBufferAttachment::dispose() const {
        vkDestroyImage(SINGLETONS.vulkanContext.device.device, vkImage, nullptr);
        vkDestroyImageView(SINGLETONS.vulkanContext.device.device, vkImageView, nullptr);
        vkFreeMemory(SINGLETONS.vulkanContext.device.device, vkImageMemory, nullptr);

        if (imageDescriptor != nullptr) {
            imageDescriptor->dispose();
        }
    }
}
