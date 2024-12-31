#include "OctreeNode.h"

namespace Metal {
    void OctreeNode::addChild(OctreeNode *child, int index) {
        children[index] = child;
    }

    void OctreeNode::prepareData() {
        childMask = 0;
        isLeafMask = 0;
        for (unsigned int i = 0; i < 8; i++) {
            if (auto &child = children[i]; child != nullptr) {
                childMask |= (1 << i);
                if (child->isLeaf) {
                    isLeafMask = 1;
                }
            }
        }
    }

    unsigned int OctreeNode::packVoxelData(unsigned int childGroupIndex) {
        prepareData();
        return (childGroupIndex << 9) | childMask << 1 | isLeafMask;
    }

    void OctreeNode::dispose() const {
        for (auto *child: children) {
            if (child != nullptr) {
                child->dispose();
                delete child;
            }
        }
    }
} // Metal
