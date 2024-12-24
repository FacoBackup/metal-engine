#ifndef SCENEDESCRIPTION_H
#define SCENEDESCRIPTION_H
#include <vector>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace Metal {
    struct EntityAssetData;

    struct SceneData final {
        std::vector<EntityAssetData> entities;
        std::string name;

        template<class Archive>
        void save(Archive &ar) const {
            ar(name, entities);
        }

        template<class Archive>
        void load(Archive &ar) {
            ar(name, entities);
        }
    };
}

#endif //SCENEDESCRIPTION_H
