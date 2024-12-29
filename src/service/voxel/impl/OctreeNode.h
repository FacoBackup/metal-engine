#ifndef OCTREENODE_H
#define OCTREENODE_H
#include <array>

#include "VoxelData.h"

namespace Metal {
    struct OctreeNode final {
        std::array<std::shared_ptr<OctreeNode>, 8> children;
        std::shared_ptr<VoxelData> data = nullptr;
        unsigned int depth = 0;

        bool isLeaf(unsigned int maxDepth) const {
            return maxDepth == depth;
        }
        /**
         * Target location of this node's data inside the SSBO buffer
         */
        unsigned int dataIndex;
        /**
         * Indicates whether the child on the index of the bit is a leaf node or not (1 bit for leaf 0 otherwise)
         */
        int isLeafMask = 0;

        /**
         * Indicates whether a child is present at the index represented by the bit set to 1
         */
        int childMask = 0;

        void addChild(const std::shared_ptr<OctreeNode> &child, int index);

        void prepareData(unsigned int targetDepth);

        /**
         * 23 bits for group index
         * 1 bit for is leaf group mask
         * 8 bits for child mask, which indicates if a child is preset at that octant
         * @param childGroupIndex
         * @param targetDepth max depth used to define if the node is a leaf or not
         * @return
         */
        unsigned int packVoxelData(unsigned int childGroupIndex, unsigned int targetDepth);
    };
} // Metal

#endif //OCTREENODE_H
