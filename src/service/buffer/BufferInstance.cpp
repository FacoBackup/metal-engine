#include "BufferInstance.h"

#include <cstring>
#include "../../service/log/LogService.h"
#include "../../context/ApplicationContext.h"
#include "../../context/vulkan/VulkanContext.h"
#include "../../util/VulkanUtils.h"

namespace Metal {
    void BufferInstance::dispose(const VulkanContext &context) const {
        LOG_INFO_S("Disposing of buffer instance");

        vkDestroyBuffer(context.device.device, vkBuffer, nullptr);
        vkFreeMemory(context.device.device, vkDeviceMemory, nullptr);
    }

    void BufferInstance::update(void *newData) const {
        if (mapped == nullptr) {
            throw std::runtime_error("BufferInstance::update() called on a non-mapped buffer");
        }
        memcpy(mapped, newData, dataSize);
    }
} // Metal
