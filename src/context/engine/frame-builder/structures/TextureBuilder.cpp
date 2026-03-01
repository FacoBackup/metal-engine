#include "TextureBuilder.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    ResourceType TextureBuilder::getType() {
        return ResourceType::TEXTURE;
    }

    RuntimeResource *TextureBuilder::build() {
        auto *texture = CTX.textureService.getResource(id);
        if (texture != nullptr) {
            return texture;
        }

        texture = CTX.textureService.createForCompute(id, w, h);
        texture->setAsNoDisposal();

        return texture;
    }
} // Metal
