#ifndef BVHBUILDERSERVICE_H
#define BVHBUILDERSERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "../data/BVH.h"
#include "../data/TLASBuffer.h"

namespace Metal {
    struct MeshData;

    struct BVHBuilderService final : AbstractRuntimeComponent {
        [[nodiscard]] BVH buildBVH() const;
    };
}

#endif
