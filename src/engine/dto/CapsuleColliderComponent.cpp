#include "CapsuleColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void CapsuleColliderComponent::registerFields() {
        registerEditableField<FLOAT>(&radius).setName("Radius").setGroup("").setMin(0.0f).setMax(100.0f);
        registerEditableField<FLOAT>(&height).setName("Height").setGroup("").setMin(0.0f).setMax(100.0f);
        registerEditableField<VECTOR3>(&offset).setName("Offset").setGroup("");
    }

    ComponentType CapsuleColliderComponent::getType() const {
        return CAPSULE_COLLIDER;
    }
}
