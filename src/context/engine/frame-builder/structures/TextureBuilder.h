#ifndef METAL_ENGINE_TEXTUREBUILDER_H
#define METAL_ENGINE_TEXTUREBUILDER_H

#include "ResourceBuilder.h"

namespace Metal {
    class TextureBuilder final : public ResourceBuilder {
        unsigned w;
        unsigned h;

    public:
        explicit TextureBuilder(const std::string &id, unsigned w, unsigned h)
            : ResourceBuilder(id), w(w), h(h) {
        }

        ResourceBuilderType getType() override;

        RuntimeResource *build() override;
    };
} // Metal

#endif //METAL_ENGINE_TEXTUREBUILDER_H
