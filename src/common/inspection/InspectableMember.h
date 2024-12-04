#ifndef METAL_ENGINE_INSPECTABLEMEMBER_H
#define METAL_ENGINE_INSPECTABLEMEMBER_H

#include <string>
#include "FieldType.h"

namespace Metal {
    struct InspectableMember {
        std::string name;
        std::string nameWithId;
        std::string group;
        FieldType type = FieldType::BOOLEAN;

        virtual ~InspectableMember() = default;
    };
}
#endif
