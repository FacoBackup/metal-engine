#ifndef METAL_ENGINE_VULKANCOREDATA_H
#define METAL_ENGINE_VULKANCOREDATA_H
#include <vulkan/vulkan.h>

namespace Metal {
    struct VulkanCoreData {
        VkAllocationCallbacks *g_Allocator = nullptr;
        VkInstance g_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice g_Device = VK_NULL_HANDLE;
        uint32_t g_QueueFamily = (uint32_t) -1;
        VkQueue g_Queue = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
        VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
        VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
    };
}
#endif
