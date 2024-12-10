#include "CoreBuffers.h"

#include "GlobalDataUBO.h"
#include "../service//BufferService.h"

namespace Metal {
    void CoreBuffers::onInitialize() {
        globalData = bufferService.createBuffer(sizeof(GlobalDataUBO),
                                                1,
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    }
}
