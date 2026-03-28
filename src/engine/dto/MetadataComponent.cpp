#include "MetadataComponent.h"

namespace Metal {
    void MetadataComponent::registerFields() {
        registerEditableField<STRING>(&name).setName("Name").setGroup("");
    }

    ComponentType MetadataComponent::getType() const {
        return METADATA;
    }
}
