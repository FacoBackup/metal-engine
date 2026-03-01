#ifndef METAL_ENGINE_TEXTUREBUILDER_H
#define METAL_ENGINE_TEXTUREBUILDER_H

#include <vulkan/vulkan_core.h>
#include "ResourceBuilder.h"

namespace Metal {
    class TextureBuilder final : public ResourceBuilder {
        unsigned w;
        unsigned h;
        VkFormat format;

    public:
        explicit TextureBuilder(const std::string &id, unsigned w, unsigned h, VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT)
            : ResourceBuilder(id), w(w), h(h), format(format) {
        }

        ResourceType getType() override;

        RuntimeResource *build() override;
    };
} // Metal

#endif //METAL_ENGINE_TEXTUREBUILDER_H
