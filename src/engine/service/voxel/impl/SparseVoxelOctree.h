#ifndef SPARSEVOXELOCTREE_H
#define SPARSEVOXELOCTREE_H
#include <unordered_map>

#include "OctreeNode.h"
#include "../../../engine-definitions.h"

namespace Metal {
    struct WorldTile;

    class SparseVoxelOctree {
        int maxDepth = 0;
        float voxelSize = 0;
        int bufferIndex = 0;
        OctreeNode root{};
        WorldTile *tile = nullptr;
        int nodeQuantity = 1;
        std::unordered_map<EntityID, unsigned long> entitiesTracked{};

        void insertInternal(OctreeNode *node, glm::vec3 &point, VoxelData &data, glm::ivec3 &position, int depth);

        void worldToChunkLocal(glm::vec3 &worldCoordinate) const;

    public:
        std::unordered_map<EntityID, unsigned long> &getEntitiesTracked() {
            return entitiesTracked;
        }

        explicit SparseVoxelOctree(WorldTile *tile);

        void setMaxDepth(unsigned int depth);

        void insert(glm::vec3 &point, VoxelData &data);
    };
} // Metal

#endif //SPARSEVOXELOCTREE_H
