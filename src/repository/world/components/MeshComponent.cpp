#include "MeshComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../enum/EntryType.h"

namespace Metal {
    void MeshComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH, false);
    }

    ComponentTypes::ComponentType MeshComponent::getType() {
        return ComponentTypes::MESH;
    }
}
