#ifndef RESOURCE_DISPOSAL_PAYLOAD_H
#define RESOURCE_DISPOSAL_PAYLOAD_H

#include "ApplicationEventContext.h"
#include <string>

namespace Metal {
    struct ResourceDisposalPayload : EventPayload {
        std::string resourceId;

        explicit ResourceDisposalPayload(std::string id) : resourceId(std::move(id)) {}
    };
}

#endif // RESOURCE_DISPOSAL_PAYLOAD_H
