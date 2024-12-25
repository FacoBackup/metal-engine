#ifndef SPARSEVOXELOCTREEDATA_H
#define SPARSEVOXELOCTREEDATA_H

#include "../../../util/serialization-definitions.h"

namespace Metal {
    struct SparseVoxelOctreeData final {
        std::vector<unsigned int> data{};

        SAVE_TEMPLATE(data)
    };
}

#endif //SPARSEVOXELOCTREEDATA_H
