#ifndef SPARSEVOXELOCTREE_H
#define SPARSEVOXELOCTREE_H
#include <unordered_map>

#include "OctreeNode.h"
#include "../../../enum/engine-definitions.h"

namespace Metal {
    struct WorldTile;

    class SparseVoxelOctreeBuilder {
        unsigned int maxDepth = 0;
        OctreeNode root{};
        unsigned int nodeQuantity = 1;
        WorldTile *tile = nullptr;

        void insertInternal(OctreeNode *node, glm::vec3 &point, const std::shared_ptr<VoxelData> &data,
                            glm::ivec3 &position, int depth);

        static void WorldToChunkLocal(const WorldTile *tile, glm::vec3 &worldCoordinate);

    public:
        explicit SparseVoxelOctreeBuilder(unsigned int maxDepth, WorldTile *tile): maxDepth(maxDepth), tile(tile) {
        }

        [[nodiscard]] unsigned int getVoxelQuantity() const {
            return nodeQuantity;
        }

        OctreeNode &getRoot() {
            return root;
        }

        WorldTile *getTile() {
            return tile;
        }

        void insert(glm::vec3 &point, const std::shared_ptr<VoxelData> &data);
    };
} // Metal

#endif //SPARSEVOXELOCTREE_H
