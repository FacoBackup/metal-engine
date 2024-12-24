#ifndef METAL_ENGINE_INSPECTABLEMEMBER_H
#define METAL_ENGINE_INSPECTABLEMEMBER_H

#include "../../enum/FieldType.h"

namespace Metal {
    struct InspectableMember {
        std::string name;
        std::string nameWithId;
        std::string id;
        std::string group;
        FieldType type = BOOLEAN;

        InspectableMember() = default;

        virtual ~InspectableMember() = default;
    };
}
#endif
