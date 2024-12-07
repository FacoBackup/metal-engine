#ifndef METAL_ENGINE_VULKANUTILS_H
#define METAL_ENGINE_VULKANUTILS_H

#include "imgui_impl_vulkan.h"
#include "../runtime/VulkanContext.h"
#include <stdio.h>
#include <stdlib.h>

namespace Metal {
    class VulkanUtils {
    public:
        static void GLFWErrorCallback(int error, const char *description);

        static void CheckVKResult(VkResult err);

        static void FramePresent(ImGui_ImplVulkanH_Window *wd, const VkQueue &graphicsQueue, bool &isSwapChainRebuild);

        static VkPhysicalDevice SelectPhysicalDevice(const VulkanContext &context);
    };
}

#endif
