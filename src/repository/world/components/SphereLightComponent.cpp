#include "SphereLightComponent.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void SphereLightComponent::registerFields() {
        registerColor(color, "", "Color");
        registerFloat(intensity, "", "Intensity", .1, 100);
        registerFloat(radiusSize, "", "Radius", .1, 10);
    }

    void SphereLightComponent::onUpdate(InspectableMember *member) {
        CTX.engineContext.setUpdateLights(true);
    }
} // Metal
