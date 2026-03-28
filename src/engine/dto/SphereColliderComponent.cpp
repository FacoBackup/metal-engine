#include "SphereColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void SphereColliderComponent::registerFields() {
        registerEditableField<FLOAT>(&radius).setName("Radius").setGroup("").setMin(0.0f).setMax(100.0f);
        registerEditableField<VECTOR3>(&offset).setName("Offset").setGroup("");
    }

    ComponentType SphereColliderComponent::getType() const {
        return SPHERE_COLLIDER;
    }
}
