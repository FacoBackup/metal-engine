#include "Entity.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../common/util/UIUtil.h"

namespace Metal {
    Entity::Entity(const EntityID id): id(id) {
        color = glm::vec3(UIUtil::DIRECTORY_COLOR.x, UIUtil::DIRECTORY_COLOR.y, UIUtil::DIRECTORY_COLOR.z);
    }

    const char *Entity::getIcon() {
        return isContainer ? Icons::inventory_2.c_str() : Icons::category.c_str();
    }
}
