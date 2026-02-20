#include "FrameBufferInstance.h"
#include "../../service/log/LogService.h"
#include "../../context/ApplicationContext.h"

namespace Metal {
    void FrameBufferInstance::dispose() {
        LOG_INFO("Disposing framebuffer instance");
        vkDestroyFramebuffer(CTX.vulkanContext.device.device, vkFramebuffer, nullptr);
        vkDestroyRenderPass(CTX.vulkanContext.device.device, vkRenderPass, nullptr);

        for (int i = 0; i < attachments.size(); i++) {
            LOG_INFO("Disposing of attachment instance " + std::to_string(i));
            attachments[i]->dispose();
        }
    }
}
