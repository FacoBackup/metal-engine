#ifndef METAL_ENGINE_VULKANUTILS_H
#define METAL_ENGINE_VULKANUTILS_H

#define MAX_FRAMES_IN_FLIGHT 2
#include <vulkan/vulkan_core.h>

namespace Metal {
    class VulkanUtils {
    public:
        static void GLFWErrorCallback(int error, const char *description);

        static void CheckVKResult(VkResult err);

        static VkFormat GetValidDepthFormat(VkPhysicalDevice physicalDevice);

        static unsigned int  GetMemTypeIndex(const VkPhysicalDeviceMemoryProperties &deviceMemProps, unsigned int  typeBits,
                                        VkFlags properties);
    };
}

#endif
