#include "MeshComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../context/ApplicationContext.h"
#include "../../../enum/EntryType.h"

namespace Metal {
    void MeshComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH);
        registerResourceSelection(materialId, "", "Material", EntryType::MATERIAL);

        registerBool(emissiveSurface, "", "Is emissive surface?");

        registerColor(albedoColor, "Material", "Albedo Color");
        registerFloat(roughnessFactor, "Material", "Roughness Factor", 0, 1, false, .001);
        registerFloat(metallicFactor, "Material", "Metallic Factor", 0, 1, false, .001);
        registerFloat(parallaxHeightScale, "Material", "Parallax height scale", 0);
        registerInt(parallaxLayers, "Material", "Parallax layers", 1);
    }

    void MeshComponent::onUpdate(InspectableMember *member, ApplicationContext &context) {
        context.engineContext.setGISettingsUpdated(true);
    }

    ComponentTypes::ComponentType MeshComponent::getType() {
        return ComponentTypes::MESH;
    }
}
