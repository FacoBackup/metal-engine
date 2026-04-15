#ifndef INSTANCED_GEOMETRY_COMPONENT_H
#define INSTANCED_GEOMETRY_COMPONENT_H

#include "AbstractComponent.h"
#include <glm/glm.hpp>

namespace Metal {
    struct InstancedGeometryComponent final : AbstractComponent {
        std::string meshId;
        std::string materialId;

        unsigned int renderIndex = 0;
        unsigned int materialIndex = 0;

        void registerFields() override;

        ComponentType getType() const override;
    };
}
#endif //INSTANCED_GEOMETRY_COMPONENT_H
