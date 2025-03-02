#ifndef BVHBUILDERSERVICE_H
#define BVHBUILDERSERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "data/BVH.h"
#include "../../dto/buffers/TLAS.h"

namespace Metal {
    struct MeshData;

    struct BVHBuilderService final : AbstractRuntimeComponent {
        explicit BVHBuilderService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        [[nodiscard]] BVH buildBVH() const;
    };
}

#endif
