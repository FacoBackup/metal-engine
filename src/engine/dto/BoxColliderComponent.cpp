#include "BoxColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void BoxColliderComponent::registerFields() {
        registerEditableField<VECTOR3>(&halfExtents).setName("halfExtents").setLabel("Half extents");
        registerEditableField<VECTOR3>(&offset).setName("offset").setLabel("Offset");
    }

    ComponentType BoxColliderComponent::getType() const {
        return BOX_COLLIDER;
    }
}
