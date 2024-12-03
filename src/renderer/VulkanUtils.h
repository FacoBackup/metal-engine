#ifndef METAL_ENGINE_VULKANUTILS_H
#define METAL_ENGINE_VULKANUTILS_H

#include "imgui_impl_vulkan.h"
#include "context/VulkanContext.h"
#include <stdio.h>
#include <stdlib.h>

namespace Metal {
    class VulkanUtils {
    public:
        static void glfw_error_callback(int error, const char *description);

        static void check_vk_result(VkResult err);

        static bool IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties, const char *extension);

        static VKAPI_ATTR VkBool32 VKAPI_CALL
        debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
                     size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage,
                     void *pUserData);


        static void FramePresent(ImGui_ImplVulkanH_Window *wd, VkQueue &g_Queue, bool &g_SwapChainRebuild);

        static VkPhysicalDevice SelectPhysicalDevice(VulkanContext &context);
    };
}

#endif
