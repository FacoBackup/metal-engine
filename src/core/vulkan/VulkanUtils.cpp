#include "../../util/VulkanUtils.h"

#include "../../../dependencies/vk-bootstrap/src/VkBootstrap.h"
#include "../ApplicationContext.h"
#include "../editor/service/LogService.h"

namespace Metal {
    void VulkanUtils::CheckVKResult(VkResult err) {
        if (err == VK_SUCCESS)
            return;
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "[runtime] Error: VkResult = %d", err);
        LOG_ERROR(buffer);
        if (err < VK_SUCCESS)
            abort();
    }

    void VulkanUtils::GLFWErrorCallback(int error, const char *description) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "GLFW Error %d: %s", error, description);
        LOG_ERROR(buffer);
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

    unsigned int VulkanUtils::GetMemTypeIndex(const VkPhysicalDeviceMemoryProperties &deviceMemProps, unsigned int typeBits,
                                          VkFlags properties) {
        for (unsigned int i = 0; i < 32; i++) {
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
