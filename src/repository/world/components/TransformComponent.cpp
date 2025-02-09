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

    void TransformComponent::onUpdate(InspectableMember *member, ApplicationContext &context) {
        bool isVolume = context.worldRepository.volumes.contains(entityId);
        bool isLight = context.worldRepository.lights.contains(entityId);
        if (isLight || isVolume) {
            context.engineContext.setLightVolumeDataNeedsUpdate(true);
        }
        if (member != nullptr && member->name == ROTATION) {
            rotation = normalize(glm::quat(rotationEuler * (glm::pi<float>() / 180.f)));
        }
        forceTransform = true;
    }
}
