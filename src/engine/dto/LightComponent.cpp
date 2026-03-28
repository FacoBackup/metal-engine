#include "LightComponent.h"

namespace Metal {
    void LightComponent::registerFields() {
        registerEditableField(&color, COLOR, "Color", "");
    }

    ComponentType LightComponent::getType() const {
        return LIGHT;
    }
} // Metal
