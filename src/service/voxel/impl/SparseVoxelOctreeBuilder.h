#ifndef SPARSEVOXELOCTREE_H
#define SPARSEVOXELOCTREE_H
#include <vector>

#include "OctreeNode.h"

#include <atomic>
#include <mutex>

namespace Metal {
    class SparseVoxelOctreeBuilder;
    struct WorldTile;
    struct VoxelData;
    struct OctreeNode;

    class SparseVoxelOctreeBuilder {
        mutable std::mutex insertMutex;
        OctreeNode root{};
        std::atomic<unsigned int> nodeQuantity{1};
        std::atomic<unsigned int> leafVoxelQuantity{0};
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

        void insert(int maxDepth, glm::vec3 point, VoxelData data);

        void dispose() const;
    };
} // Metal

#endif //SPARSEVOXELOCTREE_H
