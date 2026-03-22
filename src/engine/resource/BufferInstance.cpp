#include "BufferInstance.h"

#include "common/LoggerUtil.h"

namespace Metal {
    BufferType BufferInstance::getBufferType() {
        return bufferType;
    }

    void BufferInstance::update(void *newData, VkDeviceSize size) const {
        try {
            if (mapped == nullptr) {
                throw std::runtime_error("BufferInstance::update() called on a non-mapped buffer");
            }
            memcpy(mapped, newData, size == 0 ? dataSize : size);
        } catch (std::exception &e) {
            LOG_ERROR(std::string(" BufferInstance::update() failed: ") + e.what());
        }
    }
} // Metal
