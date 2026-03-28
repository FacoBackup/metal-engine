#ifndef METAL_ENGINE_FIELDMODIFICATIONEVENT_H
#define METAL_ENGINE_FIELDMODIFICATIONEVENT_H
#include "InspectableEventPayload.h"
#include "common/FieldMetadata.h"

namespace Metal {
    struct FieldModificationPayload : InspectableEventPayload {
        explicit FieldModificationPayload(FieldMetadata &member)
            : InspectableEventPayload(member.instance), member(member) {
        }

        FieldMetadata &member;
    };
} // Metal

#endif //METAL_ENGINE_FIELDMODIFICATIONEVENT_H
