#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include "AbstractComponent.h"
#include "../../../common/serialization-definitions.h"
#include <glm/vec3.hpp>

#include "../../../common/enum/LightVolumeType.h"

namespace Metal {
    struct LightComponent final : AbstractComponent {
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;
        float radiusSize = 1;

        int lightType = 0;
        std::string lightTypeLabel = "Sphere";

        void registerFields() override;

        ComponentTypes::ComponentType getType() override;

        void onUpdate(InspectableMember *member) override;

        SERIALIZE_TEMPLATE(color.x, color.y, color.z, entityId, intensity, radiusSize, lightType)
    };
} // Metal

#endif //LIGHTCOMPONENT_H
