#ifndef VOLUMECOMPONENT_H
#define VOLUMECOMPONENT_H

#include "AbstractComponent.h"
#include "../../../common/serialization-definitions.h"
#include <glm/vec3.hpp>

#include "../../../common/enum/LightVolumeType.h"

namespace Metal {
    struct VolumeComponent final : AbstractComponent {
        glm::vec3 albedo = glm::vec3(1.0f);
        float density = 1;
        float g = 0;
        float scatteringAlbedo = 5;
        int samples = 64;

        void registerFields() override;

        ComponentTypes::ComponentType getType() override;

        void onUpdate(InspectableMember *member) override;

        SERIALIZE_TEMPLATE(albedo.x, albedo.y, albedo.z, entityId, density, scatteringAlbedo, g, samples)
    };
} // Metal


#endif //VOLUMECOMPONENT_H
