#ifndef MATERIAL_H
#define MATERIAL_H

#include "../../common/inspection/Inspectable.h"
#include "../../util/serialization-definitions.h"

#include <glm/vec3.hpp>

namespace Metal {
    struct MaterialInfo final : Inspectable {
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

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override;

        SAVE_TEMPLATE(
            baseColor.x, baseColor.y, baseColor.z,
            subsurface,
            roughness,
            metallic,
            specular,
            specularTint,
            clearcoat,
            clearcoatGloss,
            anisotropic,
            sheen,
            sheenTint,
            isEmissive
        )

        std::vector<std::string> getDependencies() {
            return {};
        }
    };
};

#endif //MATERIAL_H
