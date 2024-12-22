#include "CoreTextures.h"

#include "../ApplicationContext.h"
#include "../runtime/TextureInstance.h"

namespace Metal {
    void CoreTextures::onInitialize() {
        brdf = context.vulkanContext.textureService.loadTexture(Util::uuidV4(), "resources/textures/brdf.png", false);
        brdf->setAsNoDisposal();
    }
} // Metal