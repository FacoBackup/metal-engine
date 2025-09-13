#ifndef SPARSEVOXELOCTREEDATA_H
#define SPARSEVOXELOCTREEDATA_H

#include <glm/vec4.hpp>

#include "../../../util/serialization-definitions.h"

namespace Metal {
    struct SparseVoxelOctreeData final {
        std::vector<unsigned int> data{};
        unsigned int voxelBufferOffset;

        SAVE_TEMPLATE(data, voxelBufferOffset)
    };
}

#endif //SPARSEVOXELOCTREEDATA_H
