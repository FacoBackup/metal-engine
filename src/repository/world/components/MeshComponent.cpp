#include "MeshComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../context/ApplicationContext.h"
#include "../../../enum/EntryType.h"

#define EMISSIVE "Is emissive surface?"

namespace Metal {
    void MeshComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH, false);
        registerResourceSelection(materialId, "", "Material", EntryType::MATERIAL, false);

        registerBool(emissiveSurface, "", EMISSIVE);

        registerColor(albedoColor, "", "Albedo Color");
        registerFloat(roughnessFactor, "", "Roughness Factor", 0, 1, false, .001);
        registerFloat(metallicFactor, "", "Metallic Factor", 0, 1, false, .001);
        registerFloat(parallaxHeightScale, "", "Parallax height scale", 0);
        registerInt(parallaxLayers, "", "Parallax layers", 1);
    }

    void MeshComponent::onUpdate(InspectableMember *member, ApplicationContext &context) {
        context.engineContext.setGISettingsUpdated(true);
    }

    ComponentTypes::ComponentType MeshComponent::getType() {
        return ComponentTypes::MESH;
    }
}
