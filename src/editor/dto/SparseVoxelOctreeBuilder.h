#ifndef SPARSEVOXELOCTREE_H
#define SPARSEVOXELOCTREE_H
#include "OctreeNode.h"

#include "engine/dto/BoundingBox.h"

namespace Metal {
    class SparseVoxelOctreeBuilder;
    struct SnapshotWorldTile;
    struct VoxelData;
    struct OctreeNode;
    struct PrimitiveComponent;

    class SparseVoxelOctreeBuilder {
        OctreeNode root{};
        unsigned int nodeQuantity{1};
        unsigned int leafVoxelQuantity{0};
        BoundingBox boundingBox;
        float size;

        void insertInternal(OctreeNode *node, glm::vec3 &point, VoxelData &data,
                            glm::ivec3 &position, int depth, int maxDepth);

        void WorldToChunkLocal(glm::vec3 &worldCoordinate);

    public:
        std::unordered_map<std::string, OctreeNode *> repeatedStructures;

        explicit SparseVoxelOctreeBuilder(BoundingBox boundingBox, float size) : boundingBox(boundingBox), size(size) {
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

        void insert(int maxDepth, glm::vec3 point, VoxelData data);

        void dispose() const;
    };
} // Metal

#endif //SPARSEVOXELOCTREE_H
