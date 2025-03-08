#ifndef BOOLEANTYPE_H
#define BOOLEANTYPE_H

#include "../../abstract/AbstractType.h"

namespace Metal {
    struct BooleanType final : AbstractType<bool> {
        IOType getType() override {
            return IOType::IO_BOOLEAN;
        }
    };
}
#endif //BOOLEANTYPE_H
