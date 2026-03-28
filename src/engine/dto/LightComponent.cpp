#include "LightComponent.h"

namespace Metal {
    void LightComponent::registerFields() {
        registerEditableField<COLOR>(&color).setName("color").setLabel("Color");
    }

    ComponentType LightComponent::getType() const {
        return LIGHT;
    }
} // Metal
