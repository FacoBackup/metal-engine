#include "VolumeComponent.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    void VolumeComponent::registerFields() {
        registerColor(albedo, "", "Albedo");
        registerFloat(density, "", "Density", .1, 10);
        registerFloat(g, "", "Phase function asymmetry (0.0 for isotropic)", 0);
        registerFloat(scatteringAlbedo, "", "Scattering albedo", 0);
        registerInt(samples, "", "Samples", 1);
    }

    ComponentTypes::ComponentType VolumeComponent::getType() {
        return ComponentTypes::VOLUME;
    }

    void VolumeComponent::onUpdate(InspectableMember *member, ApplicationContext &context) {
        context.engineContext.setLightVolumeDataNeedsUpdate(true);
    }
} // Metal
