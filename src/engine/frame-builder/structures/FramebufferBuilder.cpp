#include "FramebufferBuilder.h"
#include "../../../ApplicationContext.h"
#include "../../service/FrameBufferService.h"
#include "../../resource/FrameBufferInstance.h"

namespace Metal {
    ResourceType FramebufferBuilder::getType() {
        return ResourceType::FRAMEBUFFER;
    }

    RuntimeResource* FramebufferBuilder::build() {
        auto &framebufferService = CTX.framebufferService;
        FrameBufferInstance* fbo = framebufferService.getResource(id);
        if (fbo != nullptr) {
            return fbo;
        }

        fbo = framebufferService.createFrameBuffer(id, w, h, clearColor);

        for (const auto& attachment : attachments) {
            framebufferService.createAttachment(attachment.format, attachment.usage, fbo);
        }

        if (hasDepth) {
            framebufferService.createDepthAttachment(fbo);
        }

        framebufferService.createRenderPass(fbo);

        return fbo;
    }
} // Metal