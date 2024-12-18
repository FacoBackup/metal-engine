#include "OctreeNode.h"

namespace Metal {
    void OctreeNode::addChild(std::unique_ptr<OctreeNode> &child, int index) {
        children[index] = std::move(child);
    }

    void OctreeNode::prepareData() {
        childMask = 0;
        leafMask = 0;
        for (int i = 0; i < 8; i++) {
            if (auto &child = children[i]; child != nullptr) {
                childMask |= (1 << i);
                if (child->isLeaf) {
                    leafMask = 1;
                }
            }
        }
    }

    int OctreeNode::packVoxelData(int childGroupIndex) {
        prepareData();
        return (childGroupIndex << 9) | (leafMask << 8) | (childMask);
    }
} // Metal
