#include "MeshComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../enum/EntryType.h"

namespace Metal {
    const char *MeshComponent::getIcon() {
        return Icons::view_in_ar.c_str();
    }

    const char *MeshComponent::getTitle() {
        return "Mesh Component";
    }

    void MeshComponent::registerFields() {
        registerResourceSelection(meshId, "", "Mesh", EntryType::MESH, false);
    }
}
