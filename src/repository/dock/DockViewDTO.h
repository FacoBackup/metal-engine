#ifndef DOCKVIEWDTO_H
#define DOCKVIEWDTO_H
#include "../../util/serialization-definitions.h"

namespace Metal {
    class AbstractDockPanel;

    struct DockViewDTO final {
        AbstractDockPanel *view;
        unsigned int dockSpaceIndex;

        SERIALIZE_TEMPLATE(dockSpaceIndex)
    };
}
#endif //DOCKVIEWDTO_H
