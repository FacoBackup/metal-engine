#ifndef ABSTRACTIO_H
#define ABSTRACTIO_H

#include "script-definitions.h"

namespace Metal {
    enum IOType {
        IO_FLOW,
        IO_BOOLEAN
    };

    struct IO final {
        IOId id;
        bool isInput = false;
        IOType type;
        int offsetY = 0;
        NodeId nodeId;
        const char *name;

        explicit IO(const char *name, bool isInput, IOType type, int offsetY, NodeId nodeId) : name(name),
            id(NEW_ID),
            isInput(isInput),
            type(type),
            offsetY(offsetY),
            nodeId(std::move(nodeId)) {
        }
    };
}
#endif //ABSTRACTIO_H
