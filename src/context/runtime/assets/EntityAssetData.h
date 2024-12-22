#ifndef ENTITYASSETDATA_H
#define ENTITYASSETDATA_H
#include <string>

namespace Metal {
    struct EntityAssetData final {
        std::string name{};
        std::string meshId{};
        std::string materialId{};
        int parentEntity = -1;
        int id;

        template<class Archive>
        void serialize(Archive &ar) {
            ar(
                id,
                name,
                meshId,
                parentEntity
            );
        }
    };
}
#endif //ENTITYASSETDATA_H
