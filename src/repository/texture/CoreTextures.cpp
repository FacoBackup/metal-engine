#include "CoreTextures.h"

#include "../../context/ApplicationContext.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/texture/TextureInstance.h"
#include "../../enum/engine-definitions.h"

namespace Metal {
    void CoreTextures::onInitialize() {

        giSurfaceCache = ApplicationContext::Get().textureService.createForCompute(SURFACE_CACHE_RES, SURFACE_CACHE_RES);
        giSurfaceCache->setAsNoDisposal();

        previousFrame = ApplicationContext::Get().textureService.createForCompute(ApplicationContext::Get().vulkanContext.getWindowWidth() / ApplicationContext::Get().engineRepository.shadingResInvScale, ApplicationContext::Get().vulkanContext.getWindowHeight() / ApplicationContext::Get().engineRepository.shadingResInvScale);
        previousFrame->setAsNoDisposal();

        previousFrameMetadata = ApplicationContext::Get().textureService.createForCompute(ApplicationContext::Get().vulkanContext.getWindowWidth() / ApplicationContext::Get().engineRepository.shadingResInvScale, ApplicationContext::Get().vulkanContext.getWindowHeight() / ApplicationContext::Get().engineRepository.shadingResInvScale);
        previousFrameMetadata->setAsNoDisposal();

        currentFrame = ApplicationContext::Get().textureService.createForCompute(ApplicationContext::Get().vulkanContext.getWindowWidth() / ApplicationContext::Get().engineRepository.shadingResInvScale, ApplicationContext::Get().vulkanContext.getWindowHeight() / ApplicationContext::Get().engineRepository.shadingResInvScale);
        currentFrame->setAsNoDisposal();

    }
} // Metal
