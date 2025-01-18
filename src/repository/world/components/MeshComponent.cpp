#include "MeshComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../context/ApplicationContext.h"
#include "../../../enum/EntryType.h"

#define EMISSIVE "Is emissive surface?"

namespace Metal {
    void MeshComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH, false);
        registerResourceSelection(materialId, "", "Material", EntryType::MATERIAL, false);

        registerBool(emissiveSurface, "Behaviour", EMISSIVE);
        registerVec3(emissiveSurfaceArea, "Behaviour", "Emissive area contribution");

        registerColor(albedoColor, "Material", "Albedo Color");
        registerFloat(roughnessFactor, "Material", "Roughness Factor", 0, 1, false, .001);
        registerFloat(metallicFactor, "Material", "Metallic Factor", 0, 1, false, .001);
        registerFloat(parallaxHeightScale, "Material", "Parallax height scale", 0);
        registerInt(parallaxLayers, "Material", "Parallax layers", 1);
    }

    void MeshComponent::onUpdate(InspectableMember *member, ApplicationContext &context) {
        context.engineContext.setGISettingsUpdated(true);
        if (member != nullptr && member->name == EMISSIVE) {
            context.engineContext.setLightingDataUpdated(true);
        }
    }

    ComponentTypes::ComponentType MeshComponent::getType() {
        return ComponentTypes::MESH;
    }
}
