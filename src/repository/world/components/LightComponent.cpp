#include "LightComponent.h"

#include "../../../context/ApplicationContext.h"

namespace Metal {
    void LightComponent::registerFields() {
        registerColor(color, "", "Color");
        registerFloat(intensity, "", "Intensity", .1, 100);
        registerFloat(radius, "", "Radius", .1, 10);
    }

    ComponentTypes::ComponentType LightComponent::getType() {
        return ComponentTypes::LIGHT;
    }

    void LightComponent::onUpdate(InspectableMember *member, ApplicationContext &context) {
        context.engineContext.setLightingDataUpdated(true);
    }
} // Metal
