#ifndef MATERIALDATA_H
#define MATERIALDATA_H

#include <cstdint>
#include <glm/vec3.hpp>

namespace Metal {
    struct MaterialData {
        alignas(16) float transmission{};
        alignas(4) float thickness{};
        alignas(4) float ior{1.45f};
        alignas(4) unsigned int isEmissive{};

        alignas(4) unsigned int albedoTextureId = 0;
        alignas(4) unsigned int roughnessTextureId = 0;
        alignas(4) unsigned int metallicTextureId = 0;
    };
}

#endif //MATERIALDATA_H
