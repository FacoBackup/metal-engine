#ifndef OCTREENODE_H
#define OCTREENODE_H
#include <array>

#include "VoxelData.h"

namespace Metal {
    struct OctreeNode final {
        std::array<std::unique_ptr<OctreeNode>, 8> children;
        VoxelData data{{0, 0, 0}};
        bool isLeaf = false;

        /**
         * Target location of this node's data inside the SSBO buffer
         */
        int dataIndex;
        /**
         * Indicates whether the child on the index of the bit is a leaf node or not (1 bit for leaf 0 otherwise)
         */
        int leafMask = 0;

        /**
         * Indicates whether a child is present at the index represented by the bit set to 1
         */
        int childMask = 0;

        void addChild(std::unique_ptr<OctreeNode> &child, int index);

        void prepareData();

        /**
         * 23 bits for group index
         * 1 bit for is leaf group mask
         * 8 bits for child mask, which indicates if a child is preset at that octant
         * @param childGroupIndex
         * @return
         */
        int packVoxelData(int childGroupIndex);
    };
} // Metal

#endif //OCTREENODE_H
