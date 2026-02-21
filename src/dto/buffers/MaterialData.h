#ifndef MATERIAL_DATA_H
#define MATERIAL_DATA_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Metal {
    struct MaterialData final {
        alignas(16) glm::vec3 albedo{};
        alignas(4) float roughness{};
        alignas(4) float metallic{};
        alignas(4) unsigned int isEmissive{};

        alignas(4) unsigned int useAlbedoTexture;
        alignas(4) unsigned int useNormalTexture;
        alignas(4) unsigned int useRoughnessTexture;
        alignas(4) unsigned int useMetallicTexture;
        alignas(4) unsigned int useAoTexture;
        alignas(4) unsigned int useHeightTexture;

        alignas(4) unsigned int albedoTextureId{};
        alignas(4) unsigned int normalTextureId{};
        alignas(4) unsigned int roughnessTextureId{};
        alignas(4) unsigned int metallicTextureId{};
        alignas(4) unsigned int aoTextureId{};
        alignas(4) unsigned int heightTextureId{};
    };
}

#endif //LIGHTDATA_H
