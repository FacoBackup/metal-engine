#include "VulkanUtils.h"

namespace Metal {
    void VulkanUtils::check_vk_result(VkResult err) {
        if (err == 0)
            return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0)
            abort();
    }

    void VulkanUtils::glfw_error_callback(int error, const char *description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    bool VulkanUtils::IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties, const char *extension) {
        for (const VkExtensionProperties &p: properties)
            if (strcmp(p.extensionName, extension) == 0)
                return true;
        return false;
    }

    VkBool32
    VulkanUtils::debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
                              size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage,
                              void *pUserData) {
        (void) flags;
        (void) object;
        (void) location;
        (void) messageCode;
        (void) pUserData;
        (void) pLayerPrefix; // Unused arguments
        fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
        return VK_FALSE;
    }

    void VulkanUtils::FramePresent(ImGui_ImplVulkanH_Window *wd, VkQueue &g_Queue, bool &g_SwapChainRebuild) {
        if (g_SwapChainRebuild)
            return;
        VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &render_complete_semaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &wd->Swapchain;
        info.pImageIndices = &wd->FrameIndex;
        VkResult err = vkQueuePresentKHR(g_Queue, &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            g_SwapChainRebuild = true;
            return;
        }
        Metal::VulkanUtils::check_vk_result(err);
        wd->SemaphoreIndex =
                (wd->SemaphoreIndex + 1) % wd->SemaphoreCount; // Now we can use the next set of semaphores
    }

    VkPhysicalDevice VulkanUtils::SelectPhysicalDevice(VulkanContext &context) {
            uint32_t gpu_count;
            VkResult err = vkEnumeratePhysicalDevices(context.g_Instance, &gpu_count, nullptr);
            Metal::VulkanUtils::check_vk_result(err);
            IM_ASSERT(gpu_count > 0);

            ImVector<VkPhysicalDevice> gpus;
            gpus.resize(gpu_count);
            err = vkEnumeratePhysicalDevices(context.g_Instance, &gpu_count, gpus.Data);
            Metal::VulkanUtils::check_vk_result(err);

            // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
            // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
            // dedicated GPUs) is out of scope of this sample.
            for (VkPhysicalDevice &device: gpus) {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(device, &properties);
                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                    return device;
            }

            // Use first GPU (Integrated) is a Discrete one is not available.
            if (gpu_count > 0)
                return gpus[0];
            return VK_NULL_HANDLE;
    }
}