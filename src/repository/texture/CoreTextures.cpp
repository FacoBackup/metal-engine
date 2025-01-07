#include "CoreTextures.h"

#include "../../context/ApplicationContext.h"
#include "../../service/framebuffer/FrameBufferInstance.h"
#include "../../service/texture/TextureInstance.h"

namespace Metal {
    void CoreTextures::onInitialize() {
        brdf = context.textureService.loadTexture(Util::uuidV4(), "resources/textures/brdf.png", false);
        brdf->setAsNoDisposal();

        giSurfaceCache = context.textureService.createForCompute(2000, 2000);
        giSurfaceCache->setAsNoDisposal();

        globalIllumination = context.textureService.createForCompute(context.coreFrameBuffers.auxFBO->bufferWidth / context.engineRepository.giResScale, context.coreFrameBuffers.auxFBO->bufferHeight / context.engineRepository.giResScale);
        globalIllumination->setAsNoDisposal();

        if (context.isDebugMode()) {
            icons = context.textureService.loadTexture(Util::uuidV4(), "resources/textures/icons.png", false);
            icons->setAsNoDisposal();
        }
    }
} // Metal
