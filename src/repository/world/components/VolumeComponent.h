#ifndef VOLUMECOMPONENT_H
#define VOLUMECOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/serialization-definitions.h"
#include <glm/vec3.hpp>

#include "../../../enum/LightType.h"

namespace Metal {
    struct VolumeComponent final : AbstractComponent {
        glm::vec3 albedo = glm::vec3(1.0f);
        float density = 1;

        void registerFields() override;

        ComponentTypes::ComponentType getType() override;

        void onUpdate(InspectableMember *member, ApplicationContext &context) override;

        SERIALIZE_TEMPLATE(albedo.x, albedo.y, albedo.z, entityId, density)
    };
} // Metal


#endif //VOLUMECOMPONENT_H
