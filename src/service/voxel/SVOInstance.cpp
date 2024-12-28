#include "SVOInstance.h"

#include "../buffer/BufferInstance.h"
#include "../descriptor/DescriptorInstance.h"

namespace Metal {
    void SVOInstance::dispose(VulkanContext &context) {
        buffer->dispose(context);
    }
} // Metal