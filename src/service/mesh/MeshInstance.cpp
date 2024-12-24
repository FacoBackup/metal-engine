#include "MeshInstance.h"

#include <iostream>

#include "../buffer/BufferInstance.h"

namespace Metal {
    void MeshInstance::dispose(VulkanContext &context) {
        std::cout << "Disposing of mesh instance" << std::endl;

        indexBuffer->dispose(context);
        dataBuffer->dispose(context);

        indexBuffer = nullptr;
        dataBuffer = nullptr;
    }
} // Metal