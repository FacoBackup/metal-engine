#include "MeshInstance.h"

#include "../../service/log/LogService.h"
#include "../../context/ApplicationContext.h"

#include "../buffer/BufferInstance.h"

namespace Metal {
    void MeshInstance::dispose(VulkanContext &context) {
        LOG_INFO_S("Disposing of mesh instance");

        indexBuffer->dispose(context);
        dataBuffer->dispose(context);

        indexBuffer = nullptr;
        dataBuffer = nullptr;
    }
} // Metal