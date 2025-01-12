#ifndef SPARSEVOXELOCTREE_H
#define SPARSEVOXELOCTREE_H
#include <vector>

#include "OctreeNode.h"

namespace Metal {
    struct WorldTile;

    class SparseVoxelOctreeBuilder {
        OctreeNode root{};
        unsigned int nodeQuantity = 1;
        unsigned int leafVoxelQuantity = 0;
        WorldTile *tile = nullptr;

        void insertInternal(OctreeNode *node, glm::vec3 &point, VoxelData &data,
                            glm::ivec3 &position, int depth, int maxDepth);

        static void WorldToChunkLocal(const WorldTile *tile, glm::vec3 &worldCoordinate);

    public:
        std::unordered_map<std::string, OctreeNode *> repeatedStructures;

        explicit SparseVoxelOctreeBuilder(WorldTile *tile): tile(tile) {
        }

        [[nodiscard]] unsigned int getVoxelQuantity() const {
            return nodeQuantity;
        }

        [[nodiscard]] unsigned int getLeafVoxelQuantity() const {
            return leafVoxelQuantity;
        }

        OctreeNode &getRoot() {
            return root;
        }

        WorldTile *getTile() const {
            return tile;
        }

        void insert(int maxDepth, glm::vec3 &point, VoxelData &data);

        void dispose() const;

        void findRepeatedStructures();
    };
} // Metal

#endif //SPARSEVOXELOCTREE_H
