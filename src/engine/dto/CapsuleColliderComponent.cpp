#include "CapsuleColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void CapsuleColliderComponent::registerFields() {
        registerEditableField<FLOAT>(&radius).setName("radius").setLabel("Radius").setMin(0.0f).setMax(100.0f);
        registerEditableField<FLOAT>(&height).setName("height").setLabel("Height").setMin(0.0f).setMax(100.0f);
        registerEditableField<VECTOR3>(&offset).setName("offset").setLabel("Offset");
    }

    ComponentType CapsuleColliderComponent::getType() const {
        return CAPSULE_COLLIDER;
    }
}
