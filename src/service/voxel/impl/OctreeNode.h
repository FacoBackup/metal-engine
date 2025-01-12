#ifndef OCTREENODE_H
#define OCTREENODE_H
#include <array>

#include "VoxelData.h"

namespace Metal {
    struct OctreeNode final {
        std::array<OctreeNode *, 8> children;
        std::array<unsigned int, 2> data{};
        bool isLeaf = false;
        unsigned int depth;
        bool isStructureStart = false;
        std::string treeHashId;

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

        void addChild(OctreeNode *child, int index);

        void prepareData();

        /**
         * 23 bits for group index
         * 1 bit for is leaf group mask
         * 8 bits for child mask, which indicates if a child is preset at that octant
         * @param childGroupIndex
         * @return
         */
        unsigned int packVoxelData(unsigned int childGroupIndex);

        void dispose() const;

        std::string getId();

        void repeatedStructures(std::unordered_map<std::string, OctreeNode*> &repeated, int targetDepth);
    };
} // Metal

#endif //OCTREENODE_H
