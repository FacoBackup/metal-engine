#ifndef ANDNODE_H
#define ANDNODE_H
#include "../../abstract/AbstractNode.h"

namespace Metal {
    struct AndNode final : AbstractNode {
        AndNode();

        std::vector<IO> getIO() override;
    };
} // Metal

#endif //ANDNODE_H
