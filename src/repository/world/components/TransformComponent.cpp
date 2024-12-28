#include "TransformComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void TransformComponent::registerFields() {
        registerVec3(translation, "", "Translation");
        registerVec3(scale, "", "Scale", 1);
        registerQuat(rotation, "", "Rotation (Quaternion)");
        registerBool(isStatic, "", "Static?");
    }

    ComponentTypes::ComponentType TransformComponent::getType() {
        return ComponentTypes::TRANSFORM;
    }

    void TransformComponent::onUpdate(InspectableMember *member, ApplicationContext &context) {
        if (context.worldRepository.lights.contains(entityId)) {
            context.worldRepository.lights.at(entityId).onUpdate(member, context);
        }
    }
}
