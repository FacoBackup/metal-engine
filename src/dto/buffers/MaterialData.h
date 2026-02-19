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
        alignas(4) unsigned int albedoTexture {};
        alignas(4) unsigned int normalTexture {};
        alignas(4) unsigned int roughnessTexture {};
        alignas(4) unsigned int metallicTexture {};
        alignas(4) unsigned int aoTexture {};
    };
}

#endif //LIGHTDATA_H
