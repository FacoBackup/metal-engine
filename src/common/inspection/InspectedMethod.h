#ifndef INSPECTEDMETHOD_H
#define INSPECTEDMETHOD_H

#include <functional>
#include "InspectableMember.h"

namespace Metal {
    struct InspectedMethod final : InspectableMember {
        std::function<void()> callback;
    };
}
#endif
