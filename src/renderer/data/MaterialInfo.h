#ifndef MATERIAL_H
#define MATERIAL_H

#include "../../common/inspection/Inspectable.h"
#include "../../common/serialization-definitions.h"

#include <glm/vec3.hpp>

namespace Metal {
    struct MaterialInfo final : Inspectable {
        glm::vec3 baseColor;
        float subsurface;
        float roughness;
        float metallic;
        float specular;
        float specularTint;
        float clearcoat;
        float clearcoatGloss;
        float anisotropic;
        float sheen;
        float sheenTint;
        bool isEmissive;
        // Transparency properties
        float transmission = 0; // 0 = opaque, 1 = fully transparent
        float ior = 0; // Index of refraction
        glm::vec3 absorption;

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
            isEmissive,
            transmission,
            ior,
            absorption.x, absorption.y, absorption.z
        )

        std::vector<std::string> getDependencies() {
            return {};
        }
    };
};

#endif //MATERIAL_H
