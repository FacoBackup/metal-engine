#include "BoxColliderComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void BoxColliderComponent::registerFields() {
        registerVec3(halfExtents, "", "Half Extents");
        registerVec3(offset, "", "Offset");
    }

    ComponentType BoxColliderComponent::getType() const {
        return ComponentType::BOX_COLLIDER;
    }

    nlohmann::json BoxColliderComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        j["halfExtents"] = {halfExtents.x, halfExtents.y, halfExtents.z};
        j["offset"] = {offset.x, offset.y, offset.z};
        return j;
    }

    void BoxColliderComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
        if (j.contains("halfExtents")) {
            halfExtents = {j.at("halfExtents")[0], j.at("halfExtents")[1], j.at("halfExtents")[2]};
        }
        if (j.contains("offset")) {
            offset = {j.at("offset")[0], j.at("offset")[1], j.at("offset")[2]};
        }
    }
}
