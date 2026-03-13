#include "../../../repository/world/impl/MetadataComponent.h"
#include "../../../common/Icons.h"
#include "../../editor/util/UIUtil.h"

namespace Metal {
    void MetadataComponent::registerFields() {
        registerText(name, "", "Name");
        registerColor(color, "", "Hierarchy Color");
    }

    ComponentType MetadataComponent::getType() {
        return METADATA;
    }
}
