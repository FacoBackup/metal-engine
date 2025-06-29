#ifndef METAL_ENGINE_INSPECTABLEMEMBER_H
#define METAL_ENGINE_INSPECTABLEMEMBER_H

#include "../enum/FieldType.h"

namespace Metal {
    class Inspectable;

    struct InspectableMember {
        std::string name;
        std::string nameWithId;
        std::string id;
        std::string group;
        FieldType type = BOOLEAN;
        Inspectable *instance = nullptr;

        InspectableMember() = default;

        virtual ~InspectableMember() = default;
    };
}
#endif
