#include "FrameBufferAttachment.h"


#include "../../context/ApplicationContext.h"

namespace Metal {
    void FrameBufferAttachment::dispose() const {
        vkDestroyImage(ApplicationContext::Get().vulkanContext.device.device, vkImage, nullptr);
        vkDestroyImageView(ApplicationContext::Get().vulkanContext.device.device, vkImageView, nullptr);
        vkFreeMemory(ApplicationContext::Get().vulkanContext.device.device, vkImageMemory, nullptr);

        if (imageDescriptor != nullptr) {
            imageDescriptor->dispose();
        }
    }
}
