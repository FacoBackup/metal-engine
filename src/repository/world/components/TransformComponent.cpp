#include "TransformComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../context/ApplicationContext.h"

#define ROTATION "Rotation"

namespace Metal {
    void TransformComponent::registerFields() {
        registerVec3(translation, "", "Translation");
        registerVec3(scale, "", "Scale", 1);
        registerVec3(rotationEuler, "", ROTATION);
        registerBool(isStatic, "", "Static?");
    }

    ComponentTypes::ComponentType TransformComponent::getType() {
        return ComponentTypes::TRANSFORM;
    }

    void TransformComponent::onUpdate(InspectableMember *member) {
        bool isVolume = ApplicationContext::Get().worldRepository.volumes.contains(entityId);
        bool isLight = ApplicationContext::Get().worldRepository.lights.contains(entityId);
        if (isLight) {
            ApplicationContext::Get().engineContext.setUpdateLights(true);
        }
        if (isVolume) {
            ApplicationContext::Get().engineContext.setUpdateVolumes(true);
        }
        if (member != nullptr && member->name == ROTATION) {
            rotation = normalize(glm::quat(rotationEuler * (glm::pi<float>() / 180.f)));
        }
        forceTransform = true;
    }
}
