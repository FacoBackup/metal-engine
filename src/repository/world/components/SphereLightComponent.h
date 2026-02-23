#ifndef SPHERELIGHTCOMPONENT_H
#define SPHERELIGHTCOMPONENT_H

#include "LightComponent.h"

namespace Metal {
    struct SphereLightComponent final : LightComponent {
        float radiusSize = 1;

        LightTypes::LightType getLightType() override { return LightTypes::SPHERE; }

        void registerFields() override;

        ComponentTypes::ComponentType getType() override {
            return ComponentTypes::SPHERE_LIGHT;
        }

        nlohmann::json toJson() const override {
            nlohmann::json j = LightComponent::toJson();
            j["radiusSize"] = radiusSize;
            j["lightType"] = "SPHERE";
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            LightComponent::fromJson(j);
            radiusSize = j.at("radiusSize").get<float>();
        }
    };
} // Metal

#endif //SPHERELIGHTCOMPONENT_H
