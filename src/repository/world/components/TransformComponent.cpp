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
        bool isMesh = context.worldRepository.meshes.contains(entityId);
        bool isLight = context.worldRepository.lights.contains(entityId);
        if (isLight || isMesh) {
            if (isMesh && context.worldRepository.meshes.at(entityId).emissiveSurface || isLight) {
                context.engineContext.setLightingDataUpdated(true);
            }
        }
        if (member != nullptr && member->name == ROTATION) {
            rotation = glm::quat(rotationEuler * (glm::pi<float>() / 180.f));
        }
        forceTransform = true;
    }
}
