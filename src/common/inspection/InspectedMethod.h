#ifndef INSPECTEDMETHOD_H
#define INSPECTEDMETHOD_H

#include <functional>
#include <utility>
#include "InspectableMember.h"

namespace Metal {
    struct InspectedMethod : InspectableMember {
        std::function<void()> callback;
    };
}
#endif
