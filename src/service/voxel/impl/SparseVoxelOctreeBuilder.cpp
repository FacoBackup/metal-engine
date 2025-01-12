#include "SparseVoxelOctreeBuilder.h"
#include "../../../repository/world/impl/WorldTile.h"

namespace Metal {
    void SparseVoxelOctreeBuilder::insert(int maxDepth, glm::vec3 &point, VoxelData &data) {
        if (maxDepth < 1) {
            throw std::runtime_error("Depth is not set");
        }

        if (tile->boundingBox.intersects(point)) {
            WorldToChunkLocal(tile, point);
            auto pos = glm::ivec3{0, 0, 0};
            insertInternal(&root, point, data, pos, 0, maxDepth);
        }
    }

    void SparseVoxelOctreeBuilder::dispose() const {
        root.dispose();
    }

    void SparseVoxelOctreeBuilder::findRepeatedStructures() {
        root.repeatedStructures(repeatedStructures, 8);
    }

    void SparseVoxelOctreeBuilder::insertInternal(OctreeNode *node, glm::vec3 &point,
                                                  VoxelData &data, glm::ivec3 &position,
                                                  const int depth,
                                                  const int maxDepth) {
        node->data[0] = data.data[0];
        node->data[1] = data.data[1];


        node->depth = depth;
        if (depth == maxDepth) {
            node->isLeaf = true;
            return;
        }

        const auto size = static_cast<float>(TILE_SIZE / std::pow(2, depth));
        const auto childPos = glm::ivec3{
            point.x >= ((size * position.x) + (size / 2)) ? 1 : 0,
            point.y >= ((size * position.y) + (size / 2)) ? 1 : 0,
            point.z >= ((size * position.z) + (size / 2)) ? 1 : 0
        };
        const int childIndex = (childPos.x << 0) | (childPos.y << 1) | (childPos.z << 2);

        position.x = (position.x << 1) | childPos.x;
        position.y = (position.y << 1) | childPos.y;
        position.z = (position.z << 1) | childPos.z;

        if (node->children[childIndex] != nullptr) {
            insertInternal(node->children[childIndex], point, data, position, depth + 1, maxDepth);
        } else {
            auto *child = new OctreeNode;
            node->addChild(child, childIndex);
            insertInternal(child, point, data, position, depth + 1, maxDepth);

            if (child->depth == maxDepth - 1) {
                leafVoxelQuantity++;
            }
            // Leaf nodes don't need to be included on the tree
            if (!child->isLeaf) {
                nodeQuantity++;
            }
        }
    }

    void SparseVoxelOctreeBuilder::WorldToChunkLocal(const WorldTile *tile, glm::vec3 &worldCoordinate) {
        const float minX = tile->boundingBox.center.x - TILE_SIZE / 2.f;
        const float minY = tile->boundingBox.center.y - TILE_SIZE / 2.f;
        const float minZ = tile->boundingBox.center.z - TILE_SIZE / 2.f;

        worldCoordinate.x = worldCoordinate.x - minX;
        worldCoordinate.y = worldCoordinate.y - minY;
        worldCoordinate.z = worldCoordinate.z - minZ;
    }
} // Metal
