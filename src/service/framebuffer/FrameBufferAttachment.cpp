#include "FrameBufferAttachment.h"


#include "../../context/ApplicationContext.h"

namespace Metal {
    void FrameBufferAttachment::dispose() const {
        vkDestroyImage(CTX.vulkanContext.device.device, vkImage, nullptr);
        vkDestroyImageView(CTX.vulkanContext.device.device, vkImageView, nullptr);
        vkFreeMemory(CTX.vulkanContext.device.device, vkImageMemory, nullptr);

        if (imageDescriptor != nullptr) {
            CTX.descriptorSetService.disposeResource(imageDescriptor);
        }
    }
}
