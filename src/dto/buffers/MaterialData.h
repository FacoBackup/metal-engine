#ifndef MATERIAL_DATA_H
#define MATERIAL_DATA_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Metal {
    struct MaterialData final {
        alignas(16) glm::vec3 albedo{};
        alignas(4) float roughness {};
        alignas(4) float metallic {};

        alignas(4) bool useAlbedoTexture {};
        alignas(4) bool useNormalTexture {};
        alignas(4) bool useRoughnessTexture {};
        alignas(4) bool useMetallicTexture {};
        alignas(4) bool useAoTexture {};

        alignas(4) unsigned int albedoTextureId {};
        alignas(4) unsigned int normalTextureId {};
        alignas(4) unsigned int roughnessTextureId {};
        alignas(4) unsigned int metallicTextureId {};
        alignas(4) unsigned int aoTextureId {};
    };
}

#endif //LIGHTDATA_H
