#include "CoreTextures.h"

#include "../../context/ApplicationContext.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/texture/TextureInstance.h"
#include "../../enum/engine-definitions.h"

namespace Metal {
    void CoreTextures::onInitialize() {

        giSurfaceCache = CTX.textureService.createForCompute(SURFACE_CACHE_RES, SURFACE_CACHE_RES);
        giSurfaceCache->setAsNoDisposal();

        previousFrame = CTX.textureService.createForCompute(CTX.vulkanContext.getWindowWidth() / CTX.engineRepository.shadingResInvScale, CTX.vulkanContext.getWindowHeight() / CTX.engineRepository.shadingResInvScale);
        previousFrame->setAsNoDisposal();

        previousFrameMetadata = CTX.textureService.createForCompute(CTX.vulkanContext.getWindowWidth() / CTX.engineRepository.shadingResInvScale, CTX.vulkanContext.getWindowHeight() / CTX.engineRepository.shadingResInvScale);
        previousFrameMetadata->setAsNoDisposal();

        currentFrame = CTX.textureService.createForCompute(CTX.vulkanContext.getWindowWidth() / CTX.engineRepository.shadingResInvScale, CTX.vulkanContext.getWindowHeight() / CTX.engineRepository.shadingResInvScale);
        currentFrame->setAsNoDisposal();

    }
} // Metal
