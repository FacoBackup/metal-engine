#include "VolumeComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void VolumeComponent::registerFields() {
        registerEditableField<COLOR>(&albedo).setName("Albedo").setGroup("");
        registerEditableField<FLOAT>(&density).setName("Density").setGroup("").setMin(.1).setMax(10);
        registerEditableField<FLOAT>(&g).setName("Phase function asymmetry (0.0 for isotropic)").setGroup("").setMin(0);
        registerEditableField<FLOAT>(&scatteringAlbedo).setName("Scattering albedo").setGroup("").setMin(0);
        registerEditableField<INT>(&samples).setName("Samples").setGroup("").setMin(1);
    }

    ComponentType VolumeComponent::getType() const {
        return VOLUME;
    }
} // Metal
