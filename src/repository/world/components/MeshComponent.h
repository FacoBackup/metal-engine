#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/serialization-definitions.h"
#include <glm/glm.hpp>


namespace Metal {
    struct MeshComponent final : AbstractComponent {
        std::string meshId;
        std::string materialId;

        glm::vec3 albedoColor{1, 1, 1};
        bool emissiveSurface = false;
        float roughnessFactor = 1;
        float metallicFactor = 0;
        int parallaxLayers = 16;
        float parallaxHeightScale = 1;
        bool needsReVoxelization = false;

        void registerFields() override;

        void onUpdate(InspectableMember *member, ApplicationContext &context) override;

        ComponentTypes::ComponentType getType() override;

        SERIALIZE_TEMPLATE(entityId, meshId, materialId,
                           parallaxHeightScale, parallaxLayers,
                           emissiveSurface,
                           albedoColor.x, albedoColor.y, albedoColor.z,
                           roughnessFactor,
                           metallicFactor)
    };
}
#endif //MESHCOMPONENT_H
