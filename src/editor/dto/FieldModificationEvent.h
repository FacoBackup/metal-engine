#ifndef METAL_ENGINE_FIELDMODIFICATIONEVENT_H
#define METAL_ENGINE_FIELDMODIFICATIONEVENT_H
#include "InspectableEventPayload.h"
#include "common/InspectableMember.h"

namespace Metal {
    struct FieldModificationPayload : InspectableEventPayload {
        explicit FieldModificationPayload(InspectableMember &member)
            : InspectableEventPayload(member.instance), member(member) {
        }

        InspectableMember &member;
    };
} // Metal

#endif //METAL_ENGINE_FIELDMODIFICATIONEVENT_H
