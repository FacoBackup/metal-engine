#ifndef METAL_ENGINE_INSPECTABLEEVENTPAYLOAD_H
#define METAL_ENGINE_INSPECTABLEEVENTPAYLOAD_H
#include "ApplicationEventContext.h"

namespace Metal {
    class Inspectable;

    struct InspectableEventPayload : EventPayload {
        explicit InspectableEventPayload(Inspectable *inspectable)
            : inspectable(inspectable) {
        }

        Inspectable *inspectable;
    };
}
#endif //METAL_ENGINE_INSPECTABLEEVENTPAYLOAD_H
