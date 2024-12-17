#ifndef SPARSEVOXELOCTREE_H
#define SPARSEVOXELOCTREE_H
#include "OctreeNode.h"

namespace Metal {
    struct WorldTile;

    class SparseVoxelOctree {
        int maxDepth = 0;
        float voxelSize = 0;
        int bufferIndex = 0;
        OctreeNode root{};
        WorldTile *tile = nullptr;
        int nodeQuantity = 1;

        void insertInternal(OctreeNode *node, glm::vec3 &point, VoxelData &data, glm::ivec3 &position, int depth);

        void worldToChunkLocal(glm::vec3 &worldCoordinate) const;

    public:
        explicit SparseVoxelOctree(WorldTile *tile);

        void setMaxDepth(unsigned int depth);

        void insert(glm::vec3 &point, VoxelData &data);
    };
} // Metal

#endif //SPARSEVOXELOCTREE_H
