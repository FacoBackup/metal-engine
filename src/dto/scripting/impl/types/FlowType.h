#ifndef FLOWTYPE_H
#define FLOWTYPE_H

#include "../../abstract/AbstractType.h"

namespace Metal {
    struct FlowType final : AbstractType<short> {
        IOType getType() override {
            return IO_FLOW;
        }
    };
}
#endif //FLOWTYPE_H
