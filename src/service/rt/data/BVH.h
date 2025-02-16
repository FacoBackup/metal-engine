#ifndef BVH_H
#define BVH_H
#include <vector>

#include "BottomLevelAccelerationStructure.h"
#include "RTTriangle.h"
#include "TopLevelAccelerationStructure.h"

namespace Metal {
    struct BVH final {
        std::vector<RTTriangle> triangles;
        std::vector<TopLevelAccelerationStructure> tlas;
        std::vector<BottomLevelAccelerationStructure> blas;
    };
}

#endif //BVH_H
