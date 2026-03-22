#include "CapsuleColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void CapsuleColliderComponent::registerFields() {
        registerFloat(radius, "", "Radius", 0.0f, 100.0f);
        registerFloat(height, "", "Height", 0.0f, 100.0f);
        registerVec3(offset, "", "Offset");
    }

    ComponentType CapsuleColliderComponent::getType() const {
        return ComponentType::CAPSULE_COLLIDER;
    }

    nlohmann::json CapsuleColliderComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["radius"] = radius;
        j["height"] = height;
        j["offset"] = {offset.x, offset.y, offset.z};
        return j;
    }

    void CapsuleColliderComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
        radius = j.value("radius", 0.5f);
        height = j.value("height", 1.0f);
        if (j.contains("offset")) {
            offset = {j.at("offset")[0], j.at("offset")[1], j.at("offset")[2]};
        }
    }
}
