#include "BoxColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void BoxColliderComponent::registerFields() {
        registerEditableField<VECTOR3>(&halfExtents).setName("Half Extents").setGroup("");
        registerEditableField<VECTOR3>(&offset).setName("Offset").setGroup("");
    }

    ComponentType BoxColliderComponent::getType() const {
        return BOX_COLLIDER;
    }
}
