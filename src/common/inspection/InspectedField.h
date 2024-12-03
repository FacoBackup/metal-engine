#ifndef INSPECTEDFIELD_H
#define INSPECTEDFIELD_H

#include <any>
#include <utility>
#include "InspectableMember.h"
#include "FieldType.h"

namespace Metal {
    struct InspectedField : InspectableMember {
        std::function<void(std::any)> setValue;
        std::function<std::any()> getValue;
        FieldType type;

        std::optional<int> max;
        std::optional<int> min;
        bool disabled;

        bool isField() override { return true; }

    };
}
#endif
