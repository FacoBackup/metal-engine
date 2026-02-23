#include "CoreTextures.h"

#include "../../context/ApplicationContext.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/texture/TextureInstance.h"
#include "../../enum/engine-definitions.h"

namespace Metal {
    void CoreTextures::onInitialize() {

        surfaceCache = CTX.textureService.createForCompute(SURFACE_CACHE_RES, SURFACE_CACHE_RES);
        surfaceCache->setAsNoDisposal();

        rawRenderedFrame = CTX.textureService.createForCompute(CTX.vulkanContext.getWindowWidth() / CTX.engineRepository.shadingResInvScale, CTX.vulkanContext.getWindowHeight() / CTX.engineRepository.shadingResInvScale);
        rawRenderedFrame->setAsNoDisposal();

        accumulatedFrame = CTX.textureService.createForCompute(CTX.vulkanContext.getWindowWidth() / CTX.engineRepository.shadingResInvScale, CTX.vulkanContext.getWindowHeight() / CTX.engineRepository.shadingResInvScale);
        accumulatedFrame->setAsNoDisposal();

    }
} // Metal
