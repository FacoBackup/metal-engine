#include "SparseVoxelOctreeBuilder.h"
#include "../../../repository/world/impl/WorldTile.h"

namespace Metal {
    void SparseVoxelOctreeBuilder::setMaxDepth(const unsigned int depth) {
        this->maxDepth = depth;
        this->voxelSize = static_cast<float>(TILE_SIZE / std::pow(2, maxDepth));
    }

    void SparseVoxelOctreeBuilder::insert(const WorldTile *tile, glm::vec3 &point, VoxelData &data) {
        if (maxDepth < 1) {
            throw std::runtime_error("Depth is not set");
        }

        if (tile->boundingBox.intersects(point)) {
            WorldToChunkLocal(tile, point);
            auto pos = glm::ivec3{0, 0, 0};
            insertInternal(&root, point, data, pos, 0);
        }
    }

    void SparseVoxelOctreeBuilder::insertInternal(OctreeNode *node, glm::vec3 &point, VoxelData &data, glm::ivec3 &position,
                                           const int depth) {
        node->data = data;
        if (depth == maxDepth) {
            node->isLeaf = true;
            return;
        }

        const float size = static_cast<float>(TILE_SIZE / std::pow(2, depth));
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
            insertInternal(node->children[childIndex].get(), point, data, position, depth + 1);
        } else {
            auto child = std::make_unique<OctreeNode>();
            node->addChild(child, childIndex);
            insertInternal(child.get(), point, data, position, depth + 1);
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
