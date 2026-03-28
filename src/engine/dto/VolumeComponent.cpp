#include "VolumeComponent.h"
#include "../../ApplicationContext.h"

namespace Metal {
    void VolumeComponent::registerFields() {
        registerEditableField<COLOR>(&albedo).setName("albedo").setLabel("Albedo");
        registerEditableField<FLOAT>(&density).setName("density").setLabel("Density").setMin(.1).setMax(10);
        registerEditableField<FLOAT>(&g).setName("g").setLabel("Phase function asymmetry").setMin(0);
        registerEditableField<FLOAT>(&scatteringAlbedo).setName("scatteringAlbedo").setLabel("Scattering albedo").setMin(0);
        registerEditableField<INT>(&samples).setName("samples").setLabel("Samples").setMin(1);
    }

    ComponentType VolumeComponent::getType() const {
        return VOLUME;
    }
} // Metal
