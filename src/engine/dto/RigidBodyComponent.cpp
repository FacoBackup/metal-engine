#include "RigidBodyComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void RigidBodyComponent::registerFields() {
        registerFloat(mass, "", "Mass", 0.0f, 1000.0f);
        registerFloat(linearDamping, "", "Linear Damping", 0.0f, 10.0f);
        registerFloat(angularDamping, "", "Angular Damping", 0.0f, 10.0f);
        registerBool(useGravity, "", "Use Gravity");
        registerBool(isKinematic, "", "Is Kinematic");
    }

    ComponentType RigidBodyComponent::getType() {
        return ComponentType::RIGID_BODY;
    }

    nlohmann::json RigidBodyComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["mass"] = mass;
        j["linearDamping"] = linearDamping;
        j["angularDamping"] = angularDamping;
        j["useGravity"] = useGravity;
        j["isKinematic"] = isKinematic;
        return j;
    }

    void RigidBodyComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
        mass = j.value("mass", 1.0f);
        linearDamping = j.value("linearDamping", 0.0f);
        angularDamping = j.value("angularDamping", 0.05f);
        useGravity = j.value("useGravity", true);
        isKinematic = j.value("isKinematic", false);
    }
}
