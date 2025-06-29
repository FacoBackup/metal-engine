#ifndef MATERIALINFODATA_H
#define MATERIALINFODATA_H
#include <glm/vec3.hpp>

namespace Metal {
    struct MaterialInfoData final {
        alignas(16) glm::vec3 baseColor;
        alignas(4) float subsurface;
        alignas(4) float roughness;
        alignas(4) float metallic;
        alignas(4) float specular;
        alignas(4) float specularTint;
        alignas(4) float clearcoat;
        alignas(4) float clearcoatGloss;
        alignas(4) float anisotropic;
        alignas(4) float sheen;
        alignas(4) float sheenTint;
        alignas(4) bool isEmissive;
        alignas(4) float transmission;
        alignas(4) float ior;
        alignas(16) glm::vec3 absorption;
    };
}
#endif //MATERIALINFODATA_H
