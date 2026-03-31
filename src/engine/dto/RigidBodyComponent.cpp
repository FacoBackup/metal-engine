#include "RigidBodyComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void RigidBodyComponent::registerFields() {
        registerEditableField<FLOAT>(&mass).setName("mass").setLabel("Mass").setMin(0.0f).setMax(1000.0f);
        registerEditableField<FLOAT>(&linearDamping).setName("linearDamping").setLabel("Linear damping").setMin(0.0f).setMax(10.0f);
        registerEditableField<FLOAT>(&angularDamping).setName("angularDamping").setLabel("Angular damping").setMin(0.0f).setMax(10.0f);
        registerEditableField<BOOLEAN>(&useGravity).setName("useGravity").setLabel("Use gravity");
        registerEditableField<BOOLEAN>(&isKinematic).setName("isKinematic").setLabel("Is kinematic");
    }

    ComponentType RigidBodyComponent::getType() const {
        return RIGID_BODY;
    }
}
