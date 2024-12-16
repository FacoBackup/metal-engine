#include "TransformComponent.h"
#include "../../../../common/interface/Icons.h"

namespace Metal {
    const char *TransformComponent::getIcon() {
        return Icons::transform.c_str();
    }

    const char *TransformComponent::getTitle() {
        return "Transformation Component";
    }

    void TransformComponent::registerFields() {
        registerVec3(translation, "", "Translation");
        registerVec3(scale, "", "Scale");
        registerVec4(rotation, "", "Rotation (Quaternion)");
    }
}
