#ifndef SCENEDESCRIPTION_H
#define SCENEDESCRIPTION_H
#include <vector>
#include "../../util/serialization-definitions.h"

namespace Metal {
    struct EntityAssetData;

    struct SceneData final {
        std::vector<EntityAssetData> entities;
        std::string name;

        SAVE_TEMPLATE(name, entities)
    };
}

#endif //SCENEDESCRIPTION_H
