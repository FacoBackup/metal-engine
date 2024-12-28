#include "OctreeNode.h"

namespace Metal {
    void OctreeNode::addChild(const std::shared_ptr<OctreeNode> &child, int index) {
        children[index] = child;
    }

    void OctreeNode::prepareData() {
        childMask = 0;
        leafMask = 0;
        for (unsigned int i = 0; i < 8; i++) {
            if (auto &child = children[i]; child != nullptr) {
                childMask |= (1 << i);
                if (child->isLeaf) {
                    leafMask = 1;
                }
            }
        }
    }

    unsigned int OctreeNode::packVoxelData(unsigned int childGroupIndex) {
        prepareData();
        return (childGroupIndex << 9) | (leafMask << 8) | (childMask);
    }
} // Metal
