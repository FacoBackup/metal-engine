#include "RigidBodyComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void RigidBodyComponent::registerFields() {
        registerEditableField<FLOAT>(&mass).setName("Mass").setGroup("").setMin(0.0f).setMax(1000.0f);
        registerEditableField<FLOAT>(&linearDamping).setName("Linear Damping").setGroup("").setMin(0.0f).setMax(10.0f);
        registerEditableField<FLOAT>(&angularDamping).setName("Angular Damping").setGroup("").setMin(0.0f).setMax(10.0f);
        registerEditableField<BOOLEAN>(&useGravity).setName("Use Gravity").setGroup("");
        registerEditableField<BOOLEAN>(&isKinematic).setName("Is Kinematic").setGroup("");
    }

    ComponentType RigidBodyComponent::getType() const {
        return RIGID_BODY;
    }
}
