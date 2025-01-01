#ifndef MESHPUSHCONSTANT_H
#define MESHPUSHCONSTANT_H
#include <glm/mat4x4.hpp>

namespace Metal {
    struct MeshPushConstant final {
        glm::mat4x4 model;
        glm::vec4 albedoEmissive;
        alignas(4) unsigned int renderIndex;
        alignas(4) float roughnessFactor;
        alignas(4) float metallicFactor;
    };
}
#endif //MESHPUSHCONSTANT_H
