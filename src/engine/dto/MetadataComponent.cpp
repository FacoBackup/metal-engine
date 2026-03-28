#include "MetadataComponent.h"

namespace Metal {
    void MetadataComponent::registerFields() {
        registerEditableField<STRING>(&name).setName("name").setLabel("Name");
    }

    ComponentType MetadataComponent::getType() const {
        return METADATA;
    }
}
