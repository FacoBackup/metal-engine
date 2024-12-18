#ifndef METAL_ENGINE_VULKANUTILS_H
#define METAL_ENGINE_VULKANUTILS_H

#include "imgui_impl_vulkan.h"
#include "../../context/VulkanContext.h"
#include <stdio.h>
#include <stdlib.h>

namespace Metal {
    class VulkanUtils {
    public:
        static void GLFWErrorCallback(int error, const char *description);

        static void CheckVKResult(VkResult err);

        static void FramePresent(ImGui_ImplVulkanH_Window *wd, const VkQueue &graphicsQueue, bool &isSwapChainRebuild);

        static VkFormat GetValidDepthFormat(VkPhysicalDevice physicalDevice);

        static uint32_t GetMemTypeIndex(const VkPhysicalDeviceMemoryProperties &deviceMemProps, uint32_t typeBits,
                                        VkFlags properties);
    };
}

#endif
