#ifndef TOPLEVELACCELERATIONSTRUCTURE_H
#define TOPLEVELACCELERATIONSTRUCTURE_H
#include "../../../enum/engine-definitions.h"

namespace Metal {
    struct TopLevelAccelerationStructure final {
        alignas(16) glm::mat4x4 invTransform;
        alignas(4) unsigned int nodeOffset;
        alignas(4) unsigned int triangleOffset;
        alignas(4) EntityID id;
    };
}
#endif //TOPLEVELACCELERATIONSTRUCTURE_H
