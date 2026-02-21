#include "EntityComponent.h"
#include "../../../common/interface/Icons.h"
#include "../../../util/UIUtil.h"

namespace Metal {
    void EntityComponent::initialize(bool container) {
        this->isContainer = container;
        if (container) {
            color = glm::vec3(UIUtil::DIRECTORY_COLOR.x, UIUtil::DIRECTORY_COLOR.y, UIUtil::DIRECTORY_COLOR.z);
        } else {
            color = glm::vec3(1, 1, 1);
        }
    }

    void EntityComponent::registerFields() {
        registerText(name, "", "Name");
        registerColor(color, "", "Hierarchy Color");
    }

    const char *EntityComponent::getIcon() {
        return isContainer ? Icons::inventory_2.c_str() : Icons::category.c_str();
    }
}
