#ifndef ABSTRACTLINK_H
#define ABSTRACTLINK_H
#include <utility>

#include "IO.h"
#include "script-definitions.h"

namespace Metal {
    struct Link {
        const IOId id = NEW_ID;
        IO source;
        IO target;

        explicit Link(IO source, IO target) : source(std::move(source)), target(std::move(target)) {
        }
    };
} // Metal

#endif //ABSTRACTLINK_H
