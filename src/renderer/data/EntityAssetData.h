#ifndef ENTITYASSETDATA_H
#define ENTITYASSETDATA_H
#include <string>

#include "../../common/serialization-definitions.h"

namespace Metal {
    struct EntityAssetData final {
        std::string name{};
        std::string meshId{};
        int parentEntity = -1;
        int id;

        SERIALIZE_TEMPLATE(id, name, meshId, parentEntity)
    };
}
#endif //ENTITYASSETDATA_H
