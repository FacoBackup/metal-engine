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
        unsigned int transformOffset = 0;

        void buildBLAS(Metal::MeshData *mesh, Metal::BVH &bvh);

        void buildTLAS(const glm::mat4x4 &model, Metal::MeshData *mesh, BVH &bvh);

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
