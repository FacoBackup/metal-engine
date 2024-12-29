#include "OctreeNode.h"

namespace Metal {
    void OctreeNode::addChild(const std::shared_ptr<OctreeNode> &child, int index) {
        children[index] = child;
    }

    void OctreeNode::prepareData(unsigned int targetDepth) {
        childMask = 0;
        isLeafMask = 0;
        for (unsigned int i = 0; i < 8; i++) {
            if (auto &child = children[i]; child != nullptr) {
                childMask |= (1 << i);
                if (child->depth == targetDepth) {
                    isLeafMask = 1;
                }
            }
        }
    }

    unsigned int OctreeNode::packVoxelData(unsigned int childGroupIndex, unsigned int targetDepth) {
        prepareData(targetDepth);
        return (childGroupIndex << 9) | childMask << 1 | isLeafMask;
    }
} // Metal
