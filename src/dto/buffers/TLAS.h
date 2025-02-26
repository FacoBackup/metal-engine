#ifndef TOPLEVELACCELERATIONSTRUCTURE_H
#define TOPLEVELACCELERATIONSTRUCTURE_H
#include "../../enum/engine-definitions.h"

namespace Metal {
    /**
     *                  Top level acceleration structure
     *
     * Refers to a mesh instance. Stores the inverse of the model matrix and information
     * about the bottom level structures associated with it
     */
    struct TLAS final {
        alignas(16) glm::mat4x4 invTransform;
        alignas(4) unsigned int nodeOffset;
        alignas(4) unsigned int triangleOffset;
        alignas(4) EntityID id;
        alignas(4) MaterialID materialId;
    };
}
#endif //TOPLEVELACCELERATIONSTRUCTURE_H
