#include "FramebufferBuilder.h"
#include "../../../ApplicationContext.h"
#include "../../service/FrameBufferService.h"
#include "../../resource/FrameBufferInstance.h"

namespace Metal {
    ResourceType FramebufferBuilder::getType() {
        return FRAMEBUFFER;
    }

    RuntimeResource *FramebufferBuilder::build() {
        FrameBufferInstance *fbo = frameBufferService->getResource(id);
        if (fbo != nullptr) {
            return fbo;
        }

        fbo = frameBufferService->createFrameBuffer(id, w, h, clearColor);

        for (const auto &attachment: attachments) {
            frameBufferService->createAttachment(attachment.format, attachment.usage, fbo);
        }

        if (hasDepth) {
            frameBufferService->createDepthAttachment(fbo);
        }

        frameBufferService->createRenderPass(fbo);

        return fbo;
    }
} // Metal
