#include "CoreTextures.h"

#include "../../context/ApplicationContext.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/texture/TextureInstance.h"

namespace Metal {
    void CoreTextures::onInitialize() {

        giSurfaceCache = context.textureService.createForCompute(2000, 2000);
        giSurfaceCache->setAsNoDisposal();

        globalIllumination = context.textureService.createForCompute(context.vulkanContext.getWindowWidth() / context.engineRepository.shadingResInvScale, context.vulkanContext.getWindowHeight() / context.engineRepository.shadingResInvScale);
        globalIllumination->setAsNoDisposal();

        shading = context.textureService.createForCompute(context.vulkanContext.getWindowWidth() / context.engineRepository.shadingResInvScale, context.vulkanContext.getWindowHeight() / context.engineRepository.shadingResInvScale);
        shading->setAsNoDisposal();

    }
} // Metal
