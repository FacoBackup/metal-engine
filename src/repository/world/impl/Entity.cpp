#include "Entity.h"
#include "../../../common/interface/Icons.h"
#include "../../../util/UIUtil.h"

namespace Metal {
    void Entity::initialize(EntityID id, bool container) {
        this->id = id;
        this->isContainer = container;
        if (container) {
            color = glm::vec3(UIUtil::DIRECTORY_COLOR.x, UIUtil::DIRECTORY_COLOR.y, UIUtil::DIRECTORY_COLOR.z);
        } else {
            color = glm::vec3(1, 1, 1);
        }
    }

    void Entity::registerFields() {
        registerText(name, "", "Name");
        registerColor(color, "", "Hierarchy Color");
    }

    const char *Entity::getIcon() {
        return isContainer ? Icons::inventory_2.c_str() : Icons::category.c_str();
    }
}
