#include "SphereColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void SphereColliderComponent::registerFields() {
        registerFloat(radius, "", "Radius", 0.0f, 100.0f);
        registerVec3(offset, "", "Offset");
    }

    ComponentType SphereColliderComponent::getType() const {
        return ComponentType::SPHERE_COLLIDER;
    }

    nlohmann::json SphereColliderComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["radius"] = radius;
        j["offset"] = {offset.x, offset.y, offset.z};
        return j;
    }

    void SphereColliderComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
        radius = j.value("radius", 0.5f);
        if (j.contains("offset")) {
            offset = {j.at("offset")[0], j.at("offset")[1], j.at("offset")[2]};
        }
    }
}
