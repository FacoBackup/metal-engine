#ifndef MESHINSTANCE_H
#define MESHINSTANCE_H
#include "../../repository/abstract/RuntimeResource.h"

namespace Metal {
    struct BufferInstance;

    struct MeshInstance final : RuntimeResource {
        explicit MeshInstance(const std::string &id): RuntimeResource(id) {
        }

        std::shared_ptr<BufferInstance> dataBuffer = nullptr;
        std::shared_ptr<BufferInstance> indexBuffer = nullptr;
        uint32_t indexCount = 0;

        void dispose(VulkanContext &context) override;
    };
} // Metal

#endif //MESHINSTANCE_H