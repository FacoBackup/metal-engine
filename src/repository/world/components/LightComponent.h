#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/serialization-definitions.h"
#include <cereal/types/polymorphic.hpp>
#include <glm/vec3.hpp>

#include "../../../enum/LightType.h"

namespace Metal {
    struct LightComponent : AbstractComponent {
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;

        virtual LightTypes::LightType getLightType() = 0;

        void registerFields() override;

        ComponentTypes::ComponentType getType() override;

        void onUpdate(InspectableMember *member) override;

        SERIALIZE_TEMPLATE(entityId, color.x, color.y, color.z, intensity)
    };
} // Metal

#endif //LIGHTCOMPONENT_H
