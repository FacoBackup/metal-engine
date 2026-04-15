#ifndef PRIMITIVE_COMPONENT_H
#define PRIMITIVE_COMPONENT_H

#include "AbstractComponent.h"

#include <glm/glm.hpp>

namespace Metal {
    struct StaticGeometryComponent final : AbstractComponent {
        std::string meshId;
        std::string materialId;

        unsigned int renderIndex = 0;
        unsigned int materialIndex = 0;

        void registerFields() override;

        ComponentType getType() const override;
    };
}
#endif //MESHCOMPONENT_H
