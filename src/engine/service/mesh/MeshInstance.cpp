//
// Created by Gustavo Roque on 11/12/24.
//

#include "MeshInstance.h"

#include "../../../context/runtime/BufferInstance.h"

namespace Metal {
    void MeshInstance::dispose(VulkanContext &context) {
        indexBuffer->dispose(context);
        normalBuffer->dispose(context);
        vertexBuffer->dispose(context);
    }
} // Metal