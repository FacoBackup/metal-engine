#ifndef BVH_H
#define BVH_H
#include <vector>
#include <glm/mat4x4.hpp>

#include "../../../dto/buffers/BLAS.h"
#include "RTTriangle.h"
#include "../../../dto/buffers/TLAS.h"

namespace Metal {
    struct BVH final {
        std::vector<RTTriangle> triangles;
        std::vector<TLAS> tlas;
        std::vector<BLAS> blas;
    };
}

#endif //BVH_H
