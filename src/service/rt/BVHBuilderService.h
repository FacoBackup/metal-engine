#ifndef BVHBUILDERSERVICE_H
#define BVHBUILDERSERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "data/BVH.h"
#include "data/TopLevelAccelerationStructure.h"

namespace Metal {
    struct MeshData;

    class BVHBuilderService final : public AbstractRuntimeComponent {
        unsigned int bvhVersion = 0;

        void buildBLAS(Metal::MeshData *mesh, Metal::BVH &bvh);

        void buildTLAS(EntityID id, MeshData *mesh, BVH &bvh);

    public:
        unsigned int getBVHVersion() {
            return bvhVersion;
        }

        explicit BVHBuilderService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        BVH buildBVH();
    };
}

#endif
