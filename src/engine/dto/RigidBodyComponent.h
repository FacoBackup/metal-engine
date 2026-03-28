#ifndef RIGID_BODY_COMPONENT_H
#define RIGID_BODY_COMPONENT_H

#include "AbstractComponent.h"


namespace Metal {
    struct RigidBodyComponent final : AbstractComponent {
        float mass = 1.0f;
        float linearDamping = 0.0f;
        float angularDamping = 0.05f;
        bool useGravity = true;
        bool isKinematic = false;

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
}
#endif //RIGID_BODY_COMPONENT_H
