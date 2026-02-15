#ifndef METAL_ENGINE_SNAPSHOTWORLDTILE_H
#define METAL_ENGINE_SNAPSHOTWORLDTILE_H
#include <glm/mat4x4.hpp>

#include "../../../repository/world/impl/BoundingBox.h"

namespace Metal {
    struct MeshComponent;

    struct SnapshotWorldTile final {
        int x;
        int z;
        std::string id;
        BoundingBox boundingBox{};
    };

    struct SnapshotEntity final {
        glm::mat4x4 model;
        MeshComponent *meshComponent;
    };

    struct WorldSnapshot final {
        std::unordered_map<std::string, SnapshotWorldTile> tiles;
        std::unordered_map<std::string, std::vector<SnapshotEntity> > entitiesByTile;
    };
} // Metal

#endif //METAL_ENGINE_SNAPSHOTWORLDTILE_H
