#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/serialization-definitions.h"
#include <glm/vec3.hpp>

namespace Metal {
    struct LightComponent final : AbstractComponent {
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;
        float radius = 1;

        void registerFields() override;

        ComponentTypes::ComponentType getType() override;

        void onUpdate(InspectableMember *member, ApplicationContext &context) override;

        SERIALIZE_TEMPLATE(color.x, color.y, color.z, entityId, intensity, radius)
    };
} // Metal

#endif //LIGHTCOMPONENT_H
