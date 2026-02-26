#include "TextureBuilder.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    ResourceBuilderType TextureBuilder::getType() {
        return ResourceBuilderType::TEXTURE;
    }

    RuntimeResource *TextureBuilder::build() {
        auto texture = CTX.textureService.createForCompute(w, h);
        texture->setAsNoDisposal();

        return texture;
    }
} // Metal
