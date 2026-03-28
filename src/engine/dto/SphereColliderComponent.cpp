#include "SphereColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void SphereColliderComponent::registerFields() {
        registerEditableField<FLOAT>(&radius).setName("radius").setLabel("Radius").setMin(0.0f).setMax(100.0f);
        registerEditableField<VECTOR3>(&offset).setName("offset").setLabel("Offset");
    }

    ComponentType SphereColliderComponent::getType() const {
        return SPHERE_COLLIDER;
    }
}
