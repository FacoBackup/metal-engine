#include "TransformComponent.h"
#include "../../../common/interface/Icons.h"

namespace Metal {
    void TransformComponent::registerFields() {
        registerVec3(translation, "", "Translation");
        registerVec3(scale, "", "Scale");
        registerQuat(rotation, "", "Rotation (Quaternion)");
        registerBool(isStatic, "", "Static?");
    }

    ComponentTypes::ComponentType TransformComponent::getType() {
        return ComponentTypes::TRANSFORM;
    }
}
