#include "VolumeComponent.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void VolumeComponent::registerFields() {
    }

    ComponentTypes::ComponentType VolumeComponent::getType() {
        return ComponentTypes::VOLUME;
    }

    void VolumeComponent::onUpdate(InspectableMember *member) {
        CTX.engineContext.setUpdateVolumes(true);
    }
} // Metal
