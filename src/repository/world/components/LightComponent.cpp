#include "LightComponent.h"

#include "../../../context/ApplicationContext.h"

namespace Metal {
    void LightComponent::registerFields() {
        registerColor(color, "", "Color");
        registerFloat(intensity, "", "Intensity", .1, 100);
    }

    ComponentTypes::ComponentType LightComponent::getType() {
        return ComponentTypes::SPHERE_LIGHT;
    }

    void LightComponent::onUpdate(InspectableMember *member) {
        CTX.engineContext.setUpdateLights(true);
    }
} // Metal
