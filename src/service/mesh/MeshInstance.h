#ifndef MESHINSTANCE_H
#define MESHINSTANCE_H
#include "../../repository/abstract/RuntimeResource.h"

namespace Metal {
    struct BufferInstance;

    struct MeshInstance final : RuntimeResource {
        explicit MeshInstance(const std::string &id): RuntimeResource(id) {
        }

        void dispose() override;

        std::shared_ptr<BufferInstance> dataBuffer = nullptr;
        std::shared_ptr<BufferInstance> indexBuffer = nullptr;
        unsigned int indexCount = 0;

    };
} // Metal

#endif //MESHINSTANCE_H
