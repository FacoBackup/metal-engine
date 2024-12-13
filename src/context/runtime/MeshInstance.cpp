#include "MeshInstance.h"

#include "BufferInstance.h"

namespace Metal {
    void MeshInstance::dispose(VulkanContext &context) {
        indexBuffer->dispose(context);
        normalBuffer->dispose(context);
        vertexBuffer->dispose(context);
        uvBuffer->dispose(context);

        indexBuffer = nullptr;
        normalBuffer = nullptr;
        vertexBuffer = nullptr;
        uvBuffer = nullptr;
    }
} // Metal