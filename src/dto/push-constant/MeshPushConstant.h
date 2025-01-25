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

        alignas(4) bool useAlbedoTexture = false;
        alignas(4) bool useNormalTexture = false;
        alignas(4) bool useRoughnessTexture = false;
        alignas(4) bool useMetallicTexture = false;
        alignas(4) bool useHeightTexture = false;

        alignas(4) float parallaxHeightScale;
        alignas(4) int parallaxLayers;
    };
}
#endif //MESHPUSHCONSTANT_H
