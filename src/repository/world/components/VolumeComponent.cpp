#include "VolumeComponent.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void VolumeComponent::registerFields() {
        registerColor(albedo, "", "Albedo");
        registerFloat(density, "", "Density", .1, 10);
    }

    ComponentTypes::ComponentType VolumeComponent::getType() {
        return ComponentTypes::VOLUME;
    }

    void VolumeComponent::onUpdate(InspectableMember *member, ApplicationContext &context) {
        context.engineContext.setVolumeDataUpdated(true);
    }
} // Metal