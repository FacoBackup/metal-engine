#ifndef PLANELIGHTCOMPONENT_H
#define PLANELIGHTCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/Serializable.h"
#include <glm/vec3.hpp>

namespace Metal {
    struct PlaneLightComponent final : AbstractComponent, Serializable {
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;

        void registerFields() override;

        ComponentType getType() override {
            return ComponentType::PLANE_LIGHT;
        }

        void onUpdate(InspectableMember *member) override;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["entityId"] = entityId;
            j["color"] = {color.x, color.y, color.z};
            j["intensity"] = intensity;
            j["lightType"] = "PLANE";
            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            entityId = j.at("entityId").get<entt::entity>();
            color.x = j.at("color")[0].get<float>();
            color.y = j.at("color")[1].get<float>();
            color.z = j.at("color")[2].get<float>();
            intensity = j.at("intensity").get<float>();
        }
    };
} // Metal
#endif //PLANELIGHTCOMPONENT_H
