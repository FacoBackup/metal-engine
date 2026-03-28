#include "LightComponent.h"

namespace Metal {
    void LightComponent::registerFields() {
        registerEditableField<COLOR>(&color).setName("Color").setGroup("");
    }

    ComponentType LightComponent::getType() const {
        return LIGHT;
    }
} // Metal
