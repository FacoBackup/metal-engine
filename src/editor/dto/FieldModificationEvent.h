#ifndef METAL_ENGINE_FIELDMODIFICATIONEVENT_H
#define METAL_ENGINE_FIELDMODIFICATIONEVENT_H
#include "editor/service/EventService.h"
#include "common/InspectableMember.h"

namespace Metal {
    struct FieldModificationPayload : EventPayload {
        explicit FieldModificationPayload(InspectableMember &member)
            : member(member) {
        }

        InspectableMember &member;
    };
} // Metal

#endif //METAL_ENGINE_FIELDMODIFICATIONEVENT_H
