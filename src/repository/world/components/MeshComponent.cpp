#include "MeshComponent.h"
#include "../../../context/ApplicationContext.h"
#include "../../../enum/EntryType.h"
#include "../../../service/mesh/MeshData.h"
#include "../../../enum/LevelOfDetail.h"

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

    void MeshComponent::onUpdate(InspectableMember *member) {
        if (member != nullptr && member->name == "meshId") {
            MeshData *data = CTX.meshService.stream(meshId, LevelOfDetail::LOD_0);
            if (data != nullptr) {
                CTX.worldRepository.transforms.at(entityId).gizmoCenter = data->gizmoCenter;
                delete data;
            }
        }
        CTX.engineContext.setGISettingsUpdated(true);
        needsReVoxelization = true;
    }

    ComponentTypes::ComponentType MeshComponent::getType() {
        return ComponentTypes::MESH;
    }
}
