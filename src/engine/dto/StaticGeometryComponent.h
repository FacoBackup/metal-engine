#ifndef PRIMITIVE_COMPONENT_H
#define PRIMITIVE_COMPONENT_H

#include "AbstractComponent.h"

#include <glm/glm.hpp>

namespace Metal {
    struct StaticGeometryComponent final : AbstractComponent {
        std::string meshId;

        std::string albedo;
        std::string roughness;
        std::string metallic;
        std::string normal;
        std::string height;
        float parallaxScale = 0.05f;
        unsigned int renderIndex = 0;

        void registerFields() override;

        ComponentType getType() const override;
    };
}
#endif //MESHCOMPONENT_H
