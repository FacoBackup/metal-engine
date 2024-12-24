#ifndef SPARSEVOXELOCTREE_H
#define SPARSEVOXELOCTREE_H
#include <unordered_map>

#include "OctreeNode.h"
#include "../../../enum/engine-definitions.h"

namespace Metal {
    struct WorldTile;

    class SparseVoxelOctreeBuilder {
        int maxDepth = 0;
        float voxelSize = 0;
        int bufferIndex = 0;
        OctreeNode root{};
        int nodeQuantity = 1;
        std::unordered_map<EntityID, unsigned long> entitiesTracked{};

        void insertInternal(OctreeNode *node, glm::vec3 &point, VoxelData &data, glm::ivec3 &position, int depth);

        static void WorldToChunkLocal(const WorldTile *tile, glm::vec3 &worldCoordinate);

    public:
        std::unordered_map<EntityID, unsigned long> &getEntitiesTracked() {
            return entitiesTracked;
        }

        void setMaxDepth(unsigned int depth);

        void insert(const WorldTile *tile, glm::vec3 &point, VoxelData &data);
    };
} // Metal

#endif //SPARSEVOXELOCTREE_H
