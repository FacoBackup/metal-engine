#include "SphereLightComponent.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void SphereLightComponent::registerFields() {
        LightComponent::registerFields();
        registerFloat(radiusSize, "", "Radius", .1, 10);
    }
} // Metal
