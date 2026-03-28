#ifndef METAL_ENGINE_INSPECTABLEEVENTPAYLOAD_H
#define METAL_ENGINE_INSPECTABLEEVENTPAYLOAD_H
#include "ApplicationEventContext.h"

namespace Metal {
    class Reflection;

    struct InspectableEventPayload : EventPayload {
        explicit InspectableEventPayload(Reflection *reflectionInstance)
            : reflectionInstance(reflectionInstance) {
        }

        Reflection *reflectionInstance;
    };
}
#endif //METAL_ENGINE_INSPECTABLEEVENTPAYLOAD_H
