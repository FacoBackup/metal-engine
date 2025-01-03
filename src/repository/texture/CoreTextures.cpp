#include "CoreTextures.h"

#include "../../context/ApplicationContext.h"
#include "../../service/texture/TextureInstance.h"

namespace Metal {
    void CoreTextures::onInitialize() {
        brdf = context.textureService.loadTexture(Util::uuidV4(), "resources/textures/brdf.png", false);
        brdf->setAsNoDisposal();

        if (context.isDebugMode()) {
            icons = context.textureService.loadTexture(Util::uuidV4(), "resources/textures/icons.png", false);
            icons->setAsNoDisposal();
        }
    }
} // Metal