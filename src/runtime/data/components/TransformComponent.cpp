#include "TransformComponent.h"
#include "../../../common/enum/Icons.h"
#include "../../../MetalContextProvider.h"

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
        bool isVolume = SINGLETONS.worldRepository.volumes.contains(entityId);
        bool isLight = SINGLETONS.worldRepository.lights.contains(entityId);
        if (isLight || isVolume) {
            SINGLETONS.engineContext.setLightVolumeDataNeedsUpdate(true);
        }
        if (member != nullptr && member->name == ROTATION) {
            rotation = normalize(glm::quat(rotationEuler * (glm::pi<float>() / 180.f)));
        }
        SINGLETONS.engineContext.setWorldChange(true);
        forceTransform = true;
    }
}
