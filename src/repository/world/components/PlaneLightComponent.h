#ifndef PLANELIGHTCOMPONENT_H
#define PLANELIGHTCOMPONENT_H

#include "LightComponent.h"
#include "../../../util/Serializable.h"

namespace Metal {
    struct PlaneLightComponent final : LightComponent {
        LightTypes::LightType getLightType() override { return LightTypes::PLANE; }

        nlohmann::json toJson() const override {
            nlohmann::json j = LightComponent::toJson();
            j["lightType"] = "PLANE";
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            LightComponent::fromJson(j);
        }
    };
} // Metal
#endif //PLANELIGHTCOMPONENT_H
