#ifndef METAL_ENGINE_INSPECTABLEMEMBER_H
#define METAL_ENGINE_INSPECTABLEMEMBER_H

#include <string>

namespace Metal {
    struct InspectableMember {
        std::string name;
        std::string group;

        virtual bool isField() { return false; }
    };
}
#endif
