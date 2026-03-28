#include "RigidBodyComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void RigidBodyComponent::registerFields() {
        registerEditableField(&mass, FLOAT, "Mass", "").setMin(0.0f).setMax(1000.0f);
        registerEditableField(&linearDamping, FLOAT, "Linear Damping", "").setMin(0.0f).setMax(10.0f);
        registerEditableField(&angularDamping, FLOAT, "Angular Damping", "").setMin(0.0f).setMax(10.0f);
        registerEditableField(&useGravity, BOOLEAN, "Use Gravity", "");
        registerEditableField(&isKinematic, BOOLEAN, "Is Kinematic", "");
    }

    ComponentType RigidBodyComponent::getType() const {
        return RIGID_BODY;
    }
}
