#include "MeshComponent.h"
#include "../../../common/enum/Icons.h"
#include "../../../MetalContextProvider.h"
#include "../../../common/enum/EntryType.h"

namespace Metal {
    void MeshComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH);
        registerResourceSelection(materialId, "", "Material", EntryType::MATERIAL);
    }

    void MeshComponent::onUpdate(InspectableMember *member) {
        SINGLETONS.engineContext.setGISettingsUpdated(true);
    }

    ComponentTypes::ComponentType MeshComponent::getType() {
        return ComponentTypes::MESH;
    }
}
