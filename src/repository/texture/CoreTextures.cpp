#include "CoreTextures.h"

#include "../../context/ApplicationContext.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/texture/TextureInstance.h"
#include "../../enum/engine-definitions.h"

namespace Metal {
    void CoreTextures::onInitialize() {

        giSurfaceCache = context.textureService.createForCompute(SURFACE_CACHE_RES, SURFACE_CACHE_RES);
        giSurfaceCache->setAsNoDisposal();

        previousFrame = context.textureService.createForCompute(context.vulkanContext.getWindowWidth() / context.engineRepository.shadingResInvScale, context.vulkanContext.getWindowHeight() / context.engineRepository.shadingResInvScale);
        previousFrame->setAsNoDisposal();

        currentFrame = context.textureService.createForCompute(context.vulkanContext.getWindowWidth() / context.engineRepository.shadingResInvScale, context.vulkanContext.getWindowHeight() / context.engineRepository.shadingResInvScale);
        currentFrame->setAsNoDisposal();

    }
} // Metal
