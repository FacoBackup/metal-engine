#ifndef MESHINSTANCE_H
#define MESHINSTANCE_H
#include "RuntimeResource.h"

namespace Metal {
    struct BufferInstance;

    struct MeshInstance final : RuntimeResource {
        std::shared_ptr<BufferInstance> dataBuffer = nullptr;
        std::shared_ptr<BufferInstance> indexBuffer = nullptr;
        uint32_t indexCount = -1;

        void dispose(VulkanContext &context) override;
    };
} // Metal

#endif //MESHINSTANCE_H
