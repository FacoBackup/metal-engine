#ifndef SCENEDESCRIPTION_H
#define SCENEDESCRIPTION_H
#include <vector>

#include "SceneEntityData.h"
#include "../../common/Reflection.h"

namespace Metal {

    struct SceneData final : Reflection {
        std::vector<SceneEntityData> entities;
        std::string name;

        void registerFields() override {
            registerSerializableOnlyField(&name, STRING, "name");
            registerSerializableOnlyField(&entities, COMPOSITE, "entities");
        }
    };
}

#endif //SCENEDESCRIPTION_H
