#include "Entity.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../common/util/UIUtil.h"

namespace Metal {
    Entity::Entity(const EntityID id, const bool container): id(id) {
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
