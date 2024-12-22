#ifndef METAL_ENGINE_VULKANUTILS_H
#define METAL_ENGINE_VULKANUTILS_H

#include "../../context/VulkanContext.h"
#define MAX_FRAMES_IN_FLIGHT 2

namespace Metal {
    class VulkanUtils {
    public:
        static void GLFWErrorCallback(int error, const char *description);

        static void CheckVKResult(VkResult err);

        static VkFormat GetValidDepthFormat(VkPhysicalDevice physicalDevice);

        static uint32_t GetMemTypeIndex(const VkPhysicalDeviceMemoryProperties &deviceMemProps, uint32_t typeBits,
                                        VkFlags properties);
    };
}

#endif
