#include "BoxColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void BoxColliderComponent::registerFields() {
        registerEditableField(&halfExtents, VECTOR3, "Half Extents", "");
        registerEditableField(&offset, VECTOR3, "Offset", "");
    }

    ComponentType BoxColliderComponent::getType() const {
        return BOX_COLLIDER;
    }
}
