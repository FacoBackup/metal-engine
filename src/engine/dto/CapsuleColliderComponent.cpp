#include "CapsuleColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void CapsuleColliderComponent::registerFields() {
        registerEditableField(&radius, FLOAT, "Radius", "").setMin(0.0f).setMax(100.0f);
        registerEditableField(&height, FLOAT, "Height", "").setMin(0.0f).setMax(100.0f);
        registerEditableField(&offset, VECTOR3, "Offset", "");
    }

    ComponentType CapsuleColliderComponent::getType() const {
        return CAPSULE_COLLIDER;
    }
}
