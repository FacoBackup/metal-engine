#ifndef ABSTRACTIO_H
#define ABSTRACTIO_H

#include "script-definitions.h"
#include <imgui.h>

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
        ImVec2 circlePos{};

        explicit IO(const char *name, bool isInput, IOType type, int offsetY, NodeId nodeId) : id(NEW_ID),
            isInput(isInput),
            type(type),
            offsetY(offsetY),
            nodeId(std::move(nodeId)),
            name(name) {
        }
    };
}
#endif //ABSTRACTIO_H
