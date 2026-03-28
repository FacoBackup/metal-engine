#include "MetadataComponent.h"

namespace Metal {
    void MetadataComponent::registerFields() {
        registerEditableField(&name, STRING, "Name", "");
    }

    ComponentType MetadataComponent::getType() const {
        return METADATA;
    }
}
