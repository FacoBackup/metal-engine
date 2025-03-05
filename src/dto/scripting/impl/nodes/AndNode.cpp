#include "AndNode.h"
#include "../../IO.h"

namespace Metal {
    AndNode::AndNode() {
        name = "And";
        highlightColor = glm::vec3(1.0f, 0, 1.0f);
        width = MEDIUM_NODE_W;
        height = MEDIUM_NODE_H;
    }

    std::vector<IO> AndNode::getIO() {
        return {
            IO{"A", true, IO_BOOLEAN, 0, id},
            IO{"B", true, IO_BOOLEAN, 1, id},
            IO{"Out", false, IO_BOOLEAN, 0, id}
        };
    }
}
