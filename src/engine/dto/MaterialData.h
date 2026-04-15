#ifndef MATERIALDATA_H
#define MATERIALDATA_H

#include <cstdint>
#include <glm/vec3.hpp>

namespace Metal {
    struct MaterialData {
        alignas(4) unsigned int isEmissive{};

        alignas(4) unsigned int albedoTextureId = 0;
        alignas(4) unsigned int roughnessTextureId = 0;
        alignas(4) unsigned int metallicTextureId = 0;
        alignas(4) unsigned int normalTextureId = 0;
        alignas(4) unsigned int emissiveTextureId = 0;
        alignas(4) float roughnessFactor = 1.0f;
        alignas(4) float metallicFactor = 0.0f;
        alignas(4) float parallaxHeightScale = 0.05f;
        alignas(4) int parallaxLayers = 32;
    };
}

#endif //MATERIALDATA_H
