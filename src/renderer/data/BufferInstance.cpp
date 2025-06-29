#include "BufferInstance.h"

#include <cstring>
#include <iostream>

#include "../VulkanContext.h"
#include "../util/VulkanUtils.h"
#include "../../MetalContextProvider.h"

namespace Metal {
    void BufferInstance::dispose() const {
        std::cout << "Disposing of buffer instance" << std::endl;

        vkDestroyBuffer(SINGLETONS.vulkanContext.device.device, vkBuffer, nullptr);
        vkFreeMemory(SINGLETONS.vulkanContext.device.device, vkDeviceMemory, nullptr);
    }

    void BufferInstance::update(void *newData) const {
        if (mapped == nullptr) {
            throw std::runtime_error("BufferInstance::update() called on a non-mapped buffer");
        }
        if (newData != nullptr) {
            memcpy(mapped, newData, dataSize);
        }
    }

    void BufferInstance::update(void *newData, VkDeviceSize size) const {
        if (mapped == nullptr) {
            throw std::runtime_error("BufferInstance::update() called on a non-mapped buffer");
        }
        if (newData != nullptr) {
            memcpy(mapped, newData, size);
        }
    }
} // Metal
