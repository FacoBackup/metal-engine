#ifndef BVHBUILDERSERVICE_H
#define BVHBUILDERSERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "data/BVH.h"
#include "data/TopLevelAccelerationStructure.h"

namespace Metal {
    struct MeshData;

    class BVHBuilderService final : public AbstractRuntimeComponent {
        static int buildBLAS(MeshData *mesh, BVH &bvh);

    public:
        explicit BVHBuilderService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        BVH buildBVH() const;
    };
}

#endif
