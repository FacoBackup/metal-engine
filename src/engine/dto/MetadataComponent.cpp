#include "MetadataComponent.h"

namespace Metal {
    void MetadataComponent::registerFields() {
        registerText(name, "", "Name");
        registerColor(color, "", "Hierarchy Color");
    }

    ComponentType MetadataComponent::getType() {
        return METADATA;
    }
}
