#include "PlaneLightComponent.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void PlaneLightComponent::registerFields() {
        registerColor(color, "", "Color");
        registerFloat(intensity, "", "Intensity", .1, 100);
    }

    void PlaneLightComponent::onUpdate(InspectableMember *member) {
        CTX.engineContext.setUpdateLights(true);
    }
} // Metal
