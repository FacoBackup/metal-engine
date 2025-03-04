#include "StartNode.h"
#include "../../IO.h"

namespace Metal {
    StartNode::StartNode() {
        name = "StartNode";
        width = SMALL_NODE_W;
        height = SMALL_NODE_H;
        highlightColor = glm::vec3(1, 1, 1);
    }

    bool StartNode::isStartNode() {
        return true;
    }
}
