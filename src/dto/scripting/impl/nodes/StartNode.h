#ifndef STARTNODE_H
#define STARTNODE_H
#include "../../abstract/AbstractScriptNode.h"

namespace Metal {
    struct StartNode final : AbstractScriptNode {
        StartNode();

        bool isStartNode() override;
    };
}

#endif //STARTNODE_H
