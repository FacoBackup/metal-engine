#include "BufferInstance.h"

#include <cstring>

#include "../../common/util/VulkanUtils.h"

// THANKS TO https://github.com/blurrypiano/littleVulkanEngine/blob/master/littleVulkanEngine/tutorial19
namespace Metal {
    void BufferInstance::dispose(VulkanContext &context) {
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
