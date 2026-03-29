#include "RenderTargetBuilder.h"
#include "../../../ApplicationContext.h"
#include "../../service/RenderTargetService.h"
#include "../../resource/RenderTargetInstance.h"

namespace Metal {
    ResourceType RenderTargetBuilder::getType() {
        return RENDER_TARGET;
    }

    RuntimeResource *RenderTargetBuilder::build() {
        RenderTargetInstance *fbo = renderTargetService->getResource(id);
        if (fbo != nullptr) {
            return fbo;
        }

        fbo = renderTargetService->createRenderTarget(id, w, h, clearColor);

        for (const auto &attachment: attachments) {
            renderTargetService->createAttachment(attachment.format, attachment.usage, fbo);
        }

        if (hasDepth) {
            renderTargetService->createDepthAttachment(fbo);
        }

        return fbo;
    }
} // Metal
