#include "CoreBuffers.h"

#include "GlobalDataUBO.h"
#include "../service/core/buffer/BufferService.h"

namespace Metal {
    void CoreBuffers::onInitialize() {
        globalData = bufferService.createBuffer(BufferType::Uniform, sizeof(GlobalDataUBO),
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    }
}
