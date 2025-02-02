#include "LightComponent.h"

#include "../../../context/ApplicationContext.h"

namespace Metal {
    void LightComponent::registerFields() {
        registerColor(color, "", "Color");
        registerFloat(intensity, "", "Intensity", .1, 100);
        registerInt(lightType, "", "Light type");
        registerText(lightTypeLabel, "", "Type", true);

        registerFloat(radiusSize, "", "Radius/Size", .1, 10);
    }

    ComponentTypes::ComponentType LightComponent::getType() {
        return ComponentTypes::LIGHT;
    }

    void LightComponent::onUpdate(InspectableMember *member, ApplicationContext &context) {
        context.engineContext.setLightVolumeDataNeedsUpdate(true);
        if (member != nullptr) {
            if (lightType == 0) {
                lightTypeLabel = "Sphere";
            }
            if (lightType == 1) {
                lightTypeLabel = "Plane";
            }
        }
    }
} // Metal
