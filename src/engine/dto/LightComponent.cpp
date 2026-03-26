#include "LightComponent.h"

namespace Metal {
    void LightComponent::registerFields() {
        registerColor(color, "", "Color");
    }

    ComponentType LightComponent::getType() const {
        return LIGHT;
    }

    nlohmann::json LightComponent::toJson() const {
        nlohmann::json j;
        j["entityId"] = entityId;
        return j;
    }

    void LightComponent::fromJson(const nlohmann::json &j) {
        entityId = j.at("entityId").get<entt::entity>();
    }
} // Metal
