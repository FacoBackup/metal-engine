#include "TextureBuilder.h"
#include "../../resource/TextureInstance.h"
#include "../../service/TextureService.h"

namespace Metal {
    ResourceType TextureBuilder::getType() {
        return TEXTURE;
    }

    RuntimeResource *TextureBuilder::build() {
        auto *texture = textureService->getResource(id);
        if (texture != nullptr) {
            return texture;
        }

        texture = textureService->createForCompute(id, w, h, format);
        texture->setAsNoDisposal();

        return texture;
    }
} // Metal
