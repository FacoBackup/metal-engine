#include "SVOInstance.h"

#include "../buffer/BufferInstance.h"
#include "../descriptor/DescriptorInstance.h"

namespace Metal {
    void SVOInstance::dispose() {
        voxelsBuffer->dispose();
    }
} // Metal