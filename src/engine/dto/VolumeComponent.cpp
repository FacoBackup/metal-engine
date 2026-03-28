#include "VolumeComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void VolumeComponent::registerFields() {
        registerEditableField(&albedo, COLOR, "Albedo", "");
        registerEditableField(&density, FLOAT, "Density", "").setMin(.1).setMax(10);
        registerEditableField(&g, FLOAT, "Phase function asymmetry (0.0 for isotropic)", "").setMin(0);
        registerEditableField(&scatteringAlbedo, FLOAT, "Scattering albedo", "").setMin(0);
        registerEditableField(&samples, INT, "Samples", "").setMin(1);
    }

    ComponentType VolumeComponent::getType() const {
        return VOLUME;
    }
} // Metal
