#include "CoreTextures.h"

#include "../../MetalContextProvider.h"
#include "../data/FrameBufferInstance.h"
#include "../data/TextureInstance.h"
#include "../../common/engine-definitions.h"

namespace Metal {
    void CoreTextures::onInitialize() {

        giSurfaceCache = SINGLETONS.textureService.createForCompute(SURFACE_CACHE_RES, SURFACE_CACHE_RES);
        giSurfaceCache->setAsNoDisposal();

        previousFrame = SINGLETONS.textureService.createForCompute(SINGLETONS.vulkanContext.getWindowWidth() / SINGLETONS.engineRepository.shadingResInvScale, SINGLETONS.vulkanContext.getWindowHeight() / SINGLETONS.engineRepository.shadingResInvScale);
        previousFrame->setAsNoDisposal();

        previousFrameMetadata = SINGLETONS.textureService.createForCompute(SINGLETONS.vulkanContext.getWindowWidth() / SINGLETONS.engineRepository.shadingResInvScale, SINGLETONS.vulkanContext.getWindowHeight() / SINGLETONS.engineRepository.shadingResInvScale);
        previousFrameMetadata->setAsNoDisposal();

        currentFrame = SINGLETONS.textureService.createForCompute(SINGLETONS.vulkanContext.getWindowWidth() / SINGLETONS.engineRepository.shadingResInvScale, SINGLETONS.vulkanContext.getWindowHeight() / SINGLETONS.engineRepository.shadingResInvScale);
        currentFrame->setAsNoDisposal();

        currentPositions = SINGLETONS.textureService.createForCompute(SINGLETONS.vulkanContext.getWindowWidth() / SINGLETONS.engineRepository.shadingResInvScale, SINGLETONS.vulkanContext.getWindowHeight() / SINGLETONS.engineRepository.shadingResInvScale);
        currentPositions->setAsNoDisposal();

    }
} // Metal
