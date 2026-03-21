#ifndef RIGID_BODY_COMPONENT_H
#define RIGID_BODY_COMPONENT_H

#include "AbstractComponent.h"
#include "../../common/ISerialize.h"

namespace Metal {
    struct RigidBodyComponent final : AbstractComponent, ISerialize {
        float mass = 1.0f;
        float linearDamping = 0.0f;
        float angularDamping = 0.05f;
        bool useGravity = true;
        bool isKinematic = false;

        void registerFields() override;

        ComponentType getType() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json& j) override;
    };
}
#endif //RIGID_BODY_COMPONENT_H
