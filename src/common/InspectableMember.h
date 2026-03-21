#ifndef METAL_ENGINE_INSPECTABLEMEMBER_H
#define METAL_ENGINE_INSPECTABLEMEMBER_H

#include <string>
#include <entt/entity/entity.hpp>

#include "FieldType.h"

namespace Metal {
    class Inspectable;

    struct InspectableMember {
        std::string name;
        std::string nameWithId;
        std::string id;
        std::string group;
        std::string path;
        FieldType type = BOOLEAN;
        Inspectable *instance = nullptr;

        virtual void *getGenericPointer() {
            return nullptr;
        }

        InspectableMember() = default;

        virtual ~InspectableMember() = default;
    };
}
#endif
