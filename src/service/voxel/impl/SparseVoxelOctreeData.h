#ifndef SPARSEVOXELOCTREEDATA_H
#define SPARSEVOXELOCTREEDATA_H

#include <glm/vec4.hpp>

#include "../../../util/serialization-definitions.h"

namespace Metal {
    struct SparseVoxelOctreeData final {
        std::vector<unsigned int> data{};
        glm::vec3 tileCenter{};

        SAVE_TEMPLATE(data)
    };
}

#endif //SPARSEVOXELOCTREEDATA_H
