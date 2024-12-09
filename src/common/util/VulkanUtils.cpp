#include "VulkanUtils.h"

#include "VkBootstrap.h"

namespace Metal {
    void VulkanUtils::CheckVKResult(VkResult err) {
        if (err == VK_SUCCESS)
            return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < VK_SUCCESS)
            abort();
    }

    void VulkanUtils::GLFWErrorCallback(int error, const char *description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    void VulkanUtils::FramePresent(ImGui_ImplVulkanH_Window *wd, const VkQueue &graphicsQueue,
                                   bool &isSwapChainRebuild) {
        if (isSwapChainRebuild) {
            return;
        }
        VkSemaphore semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &semaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &wd->Swapchain;
        info.pImageIndices = &wd->FrameIndex;
        VkResult err = vkQueuePresentKHR(graphicsQueue, &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            isSwapChainRebuild = true;
            return;
        }
        CheckVKResult(err);
        wd->SemaphoreIndex =
                (wd->SemaphoreIndex + 1) % wd->SemaphoreCount; // Now we can use the next set of semaphores
    }

    VkFormat VulkanUtils::GetValidDepthFormat(VkPhysicalDevice physicalDevice) {
        VkFormat candidates[] = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };

        for (const VkFormat format: candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                return format;
            }
        }

        throw std::runtime_error("Failed to find a suitable depth format!");
    }

    uint32_t VulkanUtils::GetMemTypeIndex(const VkPhysicalDeviceMemoryProperties &deviceMemProps, uint32_t typeBits,
                                          VkFlags properties) {
        for (uint32_t i = 0; i < 32; i++) {
            if ((typeBits & 1) == 1) {
                if ((deviceMemProps.memoryTypes[i].propertyFlags & properties) == properties) {
                    return i;
                }
            }
            typeBits >>= 1;
        }
        return 0;
    }
}
