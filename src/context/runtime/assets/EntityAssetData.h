#ifndef ENTITYASSETDATA_H
#define ENTITYASSETDATA_H
#include <string>

namespace Metal {
    struct EntityAssetData final {
        std::array<float, 3> position{};
        std::array<float, 4> rotation{};
        std::array<float, 3> scale{};
        std::string name{};
        std::string meshId{};
        int parentEntity = -1;
        int id;

        template<class Archive>
        void serialize(Archive &ar) {
            ar(
                id,
                position,
                rotation,
                scale,
                name,
                meshId,
                parentEntity
            );
        }
    };
}
#endif //ENTITYASSETDATA_H
