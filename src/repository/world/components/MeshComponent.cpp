#include "MeshComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../context/ApplicationContext.h"
#include "../../../enum/EntryType.h"

namespace Metal {
    void MeshComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH);
        registerResourceSelection(materialId, "", "Material", EntryType::MATERIAL);
    }

    void MeshComponent::onUpdate(InspectableMember *member, ApplicationContext &context) {
        context.engineContext.setGISettingsUpdated(true);
    }

    ComponentTypes::ComponentType MeshComponent::getType() {
        return ComponentTypes::MESH;
    }
}
