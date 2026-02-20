#include "BufferInstance.h"

#include <cstring>
#include "../../service/log/LogService.h"
#include "../../context/ApplicationContext.h"
#include "../../context/vulkan/VulkanContext.h"
#include "../../util/VulkanUtils.h"

namespace Metal {
    void BufferInstance::dispose() const {
        LOG_INFO("Disposing of buffer instance");

        vkDestroyBuffer(CTX.vulkanContext.device.device, vkBuffer, nullptr);
        vkFreeMemory(CTX.vulkanContext.device.device, vkDeviceMemory, nullptr);
    }

    void BufferInstance::update(void *newData) const {
        if (mapped == nullptr) {
            throw std::runtime_error("BufferInstance::update() called on a non-mapped buffer");
        }
        memcpy(mapped, newData, dataSize);
    }
} // Metal
