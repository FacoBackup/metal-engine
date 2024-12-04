#ifndef INSPECTEDFIELD_H
#define INSPECTEDFIELD_H

#include <any>
#include <utility>
#include "InspectableMember.h"

namespace Metal {
    template<typename T>
    struct InspectedField : InspectableMember {
        T &field;

        std::optional<int> max;
        std::optional<int> min;
        std::optional<float> maxF;
        std::optional<float> minF;
        bool disabled = false;

        explicit InspectedField(T &field) : field(field) {}

        ~InspectedField() override = default;
    };
}
#endif
