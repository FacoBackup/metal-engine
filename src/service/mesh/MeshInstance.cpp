#include "MeshInstance.h"

#include "../../service/log/LogService.h"
#include "../../context/ApplicationContext.h"

#include "../buffer/BufferInstance.h"

namespace Metal {
    void MeshInstance::dispose() {
        LOG_INFO("Disposing of mesh instance");

        indexBuffer->dispose();
        dataBuffer->dispose();

        indexBuffer = nullptr;
        dataBuffer = nullptr;
    }
} // Metal