#ifndef INSPECTEDFIELD_H
#define INSPECTEDFIELD_H

#include "InspectableMember.h"
#include "../util/files/EntryType.h"

namespace Metal {
    template<typename T>
    struct InspectedField final : InspectableMember {
        T *field;

        std::optional<int> max;
        std::optional<int> min;
        std::optional<float> maxF;
        std::optional<float> minF;
        bool disabled = false;
        EntryType resourceType = EntryType::NONE;

        explicit InspectedField(T *field) : field(field) {}

        ~InspectedField() override = default;
    };
}
#endif
