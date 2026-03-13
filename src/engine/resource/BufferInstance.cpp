#include "../../../service/buffer/BufferInstance.h"

#include <cstring>
#include "../../editor/service/LogService.h"
#include "../../ApplicationContext.h"
#include "../../vulkan/VulkanContext.h"
#include "../../vulkan/VulkanUtils.h"

namespace Metal {
    BufferType BufferInstance::getBufferType() {
        return bufferType;
    }

    void BufferInstance::update(void *newData) const {
        if (mapped == nullptr) {
            throw std::runtime_error("BufferInstance::update() called on a non-mapped buffer");
        }
        memcpy(mapped, newData, dataSize);
    }
} // Metal
