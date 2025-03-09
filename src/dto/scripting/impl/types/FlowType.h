#ifndef FLOWTYPE_H
#define FLOWTYPE_H

#include "../../abstract/AbstractTypeNode.h"

namespace Metal {
    struct FlowType final : AbstractTypeNode {
        explicit FlowType()
            : AbstractTypeNode("FLOW") {
        }

        IOType getType() override {
            return IO_FLOW;
        }
    };
}
#endif //FLOWTYPE_H
