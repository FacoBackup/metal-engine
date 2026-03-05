#ifndef VOLUMECOMPONENT_H
#define VOLUMECOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/Serializable.h"
#include <glm/vec3.hpp>

namespace Metal {
    // TODO - (VDB) SVO BASED VOLUMES
    struct VolumeComponent final : AbstractComponent, Serializable {

        void registerFields() override;

        ComponentTypes::ComponentType getType() override;

        void onUpdate(InspectableMember *member) override;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["entityId"] = entityId;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            entityId = j.at("entityId").get<EntityID>();
        }
    };
} // Metal


#endif //VOLUMECOMPONENT_H
