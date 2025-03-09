#ifndef ANDNODE_H
#define ANDNODE_H
#include "../../abstract/AbstractScriptNode.h"

namespace Metal {
    struct AndNode final : AbstractScriptNode {
        AndNode();

        std::vector<IO> getIO() override;
    };
} // Metal

#endif //ANDNODE_H
