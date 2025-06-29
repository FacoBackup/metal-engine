#ifndef BVH_H
#define BVH_H
#include <vector>
#include <glm/mat4x4.hpp>

#include "BLASBuffer.h"
#include "TriangleBuffer.h"
#include "TLASBuffer.h"

namespace Metal {
    struct BVH final {
        std::vector<TriangleBuffer> triangles;
        std::vector<TLASBuffer> tlas;
        std::vector<BLASBuffer> blas;
    };
}

#endif //BVH_H
