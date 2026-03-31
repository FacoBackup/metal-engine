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
        alignas(4) unsigned int heightTextureId = 0;
        alignas(4) float parallaxScale = 0.05f;
    };
}

#endif //MATERIALDATA_H
