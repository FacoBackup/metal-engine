#ifndef BOOLEANTYPE_H
#define BOOLEANTYPE_H

#include "../../abstract/AbstractTypeNode.h"

namespace Metal {
    struct BooleanType final : AbstractTypeNode {
        explicit BooleanType(const std::string &key)
            : AbstractTypeNode(key) {
        }

        IOType getType() override {
            return IOType::IO_BOOLEAN;
        }
    };
}
#endif //BOOLEANTYPE_H
