#include "MetadataComponent.h"

namespace Metal {
    void MetadataComponent::registerFields() {
        registerText(name, "", "Name");
    }

    ComponentType MetadataComponent::getType() const {
        return METADATA;
    }
}
