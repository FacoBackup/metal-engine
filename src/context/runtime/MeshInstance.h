#ifndef MESHINSTANCE_H
#define MESHINSTANCE_H
#include "RuntimeResource.h"

namespace Metal {
    struct BufferInstance;

    struct MeshInstance final : RuntimeResource {
        std::shared_ptr<BufferInstance> vertexBuffer = nullptr;
        std::shared_ptr<BufferInstance> indexBuffer = nullptr;
        std::shared_ptr<BufferInstance> normalBuffer = nullptr;
        std::shared_ptr<BufferInstance> uvBuffer = nullptr;

        void dispose(VulkanContext &context) override;
    };
} // Metal

#endif //MESHINSTANCE_H
