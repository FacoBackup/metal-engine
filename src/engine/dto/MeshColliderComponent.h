#ifndef MESH_COLLIDER_COMPONENT_H
#define MESH_COLLIDER_COMPONENT_H

#include <string>
#include "AbstractComponent.h"


namespace Metal {
    struct MeshColliderComponent final : AbstractComponent {
        std::string meshId;
        bool isConvex = false;

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
}
#endif //MESH_COLLIDER_COMPONENT_H
