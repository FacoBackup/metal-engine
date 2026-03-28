#include "SphereColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void SphereColliderComponent::registerFields() {
        registerEditableField(&radius, FLOAT, "Radius", "").setMin(0.0f).setMax(100.0f);
        registerEditableField(&offset, VECTOR3, "Offset", "");
    }

    ComponentType SphereColliderComponent::getType() const {
        return SPHERE_COLLIDER;
    }
}
