#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "../impl/AbstractComponent.h"
#include "../../../util/serialization-definitions.h"
#include <glm/glm.hpp>

namespace Metal {
    struct MeshComponent final : AbstractComponent {
        std::string meshId;

        std::string albedo;
        bool useAlbedoTexture = false;
        std::string normal;
        bool useNormalTexture = false;
        std::string roughness;
        bool useRoughnessTexture = false;
        std::string metallic;
        bool useMetallicTexture = false;
        std::string ao;
        bool useAOTexture = false;
        std::string height;
        bool useHeightTexture = false;

        glm::vec3 albedoColor{1, 1, 1};
        bool emissiveSurface = false;
        float roughnessFactor = 1;
        float metallicFactor = .5;

        void registerFields() override;

        void onUpdate(InspectableMember *member, ApplicationContext &context) override;

        ComponentTypes::ComponentType getType() override;

        SERIALIZE_TEMPLATE(entityId,
                           emissiveSurface,
                           meshId,
                           albedo,
                           useAlbedoTexture,
                           normal,
                           useNormalTexture,
                           roughness,
                           useRoughnessTexture,
                           metallic,
                           useMetallicTexture,
                           ao,
                           useAOTexture,
                           height,
                           useHeightTexture,
                           albedoColor.x, albedoColor.y, albedoColor.z,
                           roughnessFactor,
                           metallicFactor
        )
    };
}
#endif //MESHCOMPONENT_H
