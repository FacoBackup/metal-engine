#include "CoreTextures.h"

#include "../../MetalContextProvider.h"
#include "../data/FrameBufferInstance.h"
#include "../data/TextureInstance.h"
#include "../../common/engine-definitions.h"

namespace Metal {
    void CoreTextures::onInitialize() {
        giSurfaceCache = SINGLETONS.textureService.createForCompute(SURFACE_CACHE_RES, SURFACE_CACHE_RES);
        giSurfaceCache->setAsNoDisposal();

        previousFrame = SINGLETONS.textureService.createForCompute(
            SINGLETONS.vulkanContext.getWindowWidth() / SINGLETONS.engineRepository.shadingResInvScale,
            SINGLETONS.vulkanContext.getWindowHeight() / SINGLETONS.engineRepository.shadingResInvScale);
        previousFrame->setAsNoDisposal();

        previousFrameMetadata = SINGLETONS.textureService.createForCompute(
            SINGLETONS.vulkanContext.getWindowWidth() / SINGLETONS.engineRepository.shadingResInvScale,
            SINGLETONS.vulkanContext.getWindowHeight() / SINGLETONS.engineRepository.shadingResInvScale);
        previousFrameMetadata->setAsNoDisposal();

        current1Frame = SINGLETONS.textureService.createForCompute(
            SINGLETONS.vulkanContext.getWindowWidth() ,
            SINGLETONS.vulkanContext.getWindowHeight());
        current1Frame->setAsNoDisposal();

        current2Frame = SINGLETONS.textureService.createForCompute(
            SINGLETONS.vulkanContext.getWindowWidth() / SINGLETONS.engineRepository.shadingResInvScale,
            SINGLETONS.vulkanContext.getWindowHeight() / SINGLETONS.engineRepository.shadingResInvScale);
        current2Frame->setAsNoDisposal();

        currentPositions = SINGLETONS.textureService.createForCompute(
            SINGLETONS.vulkanContext.getWindowWidth() / SINGLETONS.engineRepository.shadingResInvScale,
            SINGLETONS.vulkanContext.getWindowHeight() / SINGLETONS.engineRepository.shadingResInvScale);
        currentPositions->setAsNoDisposal();
    }
} // Metal
