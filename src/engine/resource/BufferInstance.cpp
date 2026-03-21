#include "BufferInstance.h"

namespace Metal {
    BufferType BufferInstance::getBufferType() {
        return bufferType;
    }

    void BufferInstance::update(void *newData, VkDeviceSize size) const {
        if (mapped == nullptr) {
            throw std::runtime_error("BufferInstance::update() called on a non-mapped buffer");
        }
        memcpy(mapped, newData, size == 0 ? dataSize : size);
    }
} // Metal
