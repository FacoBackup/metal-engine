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

    std::string OctreeNode::getId() {
        if (treeHashId.empty()) {
            prepareData();
            char charData = static_cast<char>(childMask | isLeaf);
            std::string id;
            id += charData;
            if (isLeaf) {
                id += std::to_string(data->data[0]);
                id += std::to_string(data->data[1]);
            } else {
                for (int i = 0; i < 8; i++) {
                    if (auto &child = children[i]; child != nullptr) {
                        id += child->getId();
                    }
                }
            }
            treeHashId = id;
        }
        return treeHashId;
    }

    void OctreeNode::repeatedStructures(std::unordered_map<std::string, OctreeNode *> &repeated,
                                        int targetDepth) {
        if (depth == targetDepth) {
            isStructureStart = true;
            std::string identifier = getId();
            if (!repeated.contains(identifier)) {
                repeated.insert({identifier, this});
            }
        } else {
            for (auto *child: children) {
                if (child != nullptr) {
                    child->repeatedStructures(repeated, targetDepth);
                }
            }
        }
    }
} // Metal
