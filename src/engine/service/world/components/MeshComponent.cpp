#include "MeshComponent.h"
#include "../../../../common/interface/Icons.h"

namespace Metal {
    const char *MeshComponent::getIcon() {
        return Icons::category.c_str();
    }

    const char *MeshComponent::getTitle() {
        return "Mesh Component";
    }

    void MeshComponent::registerFields() {


    }
}
