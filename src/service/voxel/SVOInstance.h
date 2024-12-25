#ifndef SVOINSTANCE_H
#define SVOINSTANCE_H
#include "../../repository/abstract/RuntimeResource.h"
#include "../../service/buffer/BufferInstance.h"

namespace Metal {
    struct DescriptorInstance;
    /**
     * Sparse voxel octree instance
     * Stores reference to vulkan descriptor set
     */
    struct SVOInstance final : RuntimeResource {
        std::shared_ptr<BufferInstance> buffer = nullptr;

        explicit SVOInstance(const std::string &id) : RuntimeResource(id) {
        }

        void dispose(VulkanContext &context) override;
    };
} // Metal

#endif //SVOINSTANCE_H
