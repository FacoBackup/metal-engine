#ifndef SVOINSTANCE_H
#define SVOINSTANCE_H
#include "RuntimeResource.h"
#include "BufferInstance.h"

namespace Metal {
    struct DescriptorInstance;
    /**
     * Sparse voxel octree instance
     */
    struct SVOInstance final : RuntimeResource {
        BufferInstance *voxelsBuffer = nullptr;
        unsigned int voxelBufferOffset = 0;

        explicit SVOInstance(const std::string &id) : RuntimeResource(id) {
        }
    };
} // Metal

#endif //SVOINSTANCE_H
