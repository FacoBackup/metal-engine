#include "MeshInstance.h"

#include "BufferInstance.h"

namespace Metal {
    void MeshInstance::dispose(VulkanContext &context) {
        indexBuffer->dispose(context);
        dataBuffer->dispose(context);

        indexBuffer = nullptr;
        dataBuffer = nullptr;
    }
} // Metal