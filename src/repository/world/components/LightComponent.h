#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/Serializable.h"
#include <glm/vec3.hpp>

#include "../../../enum/LightType.h"

namespace Metal {
    struct LightComponent : AbstractComponent, Serializable {
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;

        virtual LightTypes::LightType getLightType() = 0;

        void registerFields() override;

        void onUpdate(InspectableMember *member) override;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["entityId"] = entityId;
            j["color"] = {color.x, color.y, color.z};
            j["intensity"] = intensity;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            entityId = j.at("entityId").get<EntityID>();
            color.x = j.at("color")[0].get<float>();
            color.y = j.at("color")[1].get<float>();
            color.z = j.at("color")[2].get<float>();
            intensity = j.at("intensity").get<float>();
        }
    };
} // Metal

#endif //LIGHTCOMPONENT_H
