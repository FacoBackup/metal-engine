#include "TransformComponent.h"
#include "VolumeComponent.h"
#include "LightComponent.h"
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
        const auto e = static_cast<entt::entity>(entityId);
        bool isVolume = CTX.worldRepository.registry.all_of<VolumeComponent>(e);
        bool isLight = CTX.worldRepository.registry.all_of<std::unique_ptr<LightComponent>>(e);
        if (isLight) {
            CTX.engineContext.setUpdateLights(true);
        }
        if (isVolume) {
            CTX.engineContext.setUpdateVolumes(true);
        }
        if (member != nullptr && member->name == ROTATION) {
            rotation = normalize(glm::quat(rotationEuler * (glm::pi<float>() / 180.f)));
        }
        forceTransform = true;
    }
}
