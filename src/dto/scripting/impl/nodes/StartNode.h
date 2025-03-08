#ifndef STARTNODE_H
#define STARTNODE_H
#include "../../abstract/AbstractNode.h"

namespace Metal {
    struct StartNode final : AbstractNode {
        StartNode();

        bool isStartNode() override;
    };
}

#endif //STARTNODE_H
