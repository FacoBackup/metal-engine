#include "VulkanContext.h"

#include <iostream>

#include "VkBootstrap.h"
#include "../VulkanUtils.h"
#include "../../common/runtime/AbstractRuntimeComponent.h"

#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

namespace Metal {
    void VulkanContext::shutdown() const {
        vkDestroyDescriptorPool(device.device, descriptorPool,
                                instance.allocation_callbacks);
        vkb::destroy_device(device);
        vkb::destroy_instance(instance);
    }

    void VulkanContext::build(const bool debugMode) {
        this->debugMode = debugMode;
        g_MainWindowData.ClearValue.color.float32[0] = 0;
        g_MainWindowData.ClearValue.color.float32[1] = 0;
        g_MainWindowData.ClearValue.color.float32[2] = 0;
        g_MainWindowData.ClearValue.color.float32[3] = 0;
    }

    void VulkanContext::createSwapChain() {
        glfwGetFramebufferSize(window, &w, &h);
        IM_ASSERT(MIN_IMAGE_COUNT >= 2);
        ImGui_ImplVulkanH_CreateOrResizeWindow(instance.instance, physDevice.physical_device,
                                               device.device, &g_MainWindowData, queueFamily,
                                               instance.allocation_callbacks,
                                               w, h, MIN_IMAGE_COUNT);
        swapChain = g_MainWindowData.Swapchain;
    }

    void VulkanContext::createQueue() {
        auto queueResult = device.get_queue(vkb::QueueType::graphics);
        if (!queueResult) {
            throw std::runtime_error("Failed to create queue.");
        }
        graphicsQueue = queueResult.value();
        std::vector<vkb::CustomQueueDescription> queue_descriptions;
        auto queue_families = physDevice.get_queue_families();
        bool found = false;
        for (uint32_t i = 0; i < static_cast<uint32_t>(queue_families.size()); i++) {
            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                found = true;
                queueFamily = i;
                break;
            }
        }
        if (!found) {
            throw std::runtime_error("Failed to find necessary queue family.");
        }
    }

    void VulkanContext::createDevice() {
        const vkb::DeviceBuilder deviceBuilder{physDevice};
        auto deviceResult = deviceBuilder.build();
        if (!deviceResult) {
            throw std::runtime_error("Failed to create device.");
        }
        device = deviceResult.value();
    }

    void VulkanContext::createPhysicalDevice() {
        vkb::PhysicalDeviceSelector physDeviceSelector(instance);
        VkPhysicalDeviceFeatures features{};
        features.tessellationShader = true;

        // NOT SUPPORTED ON MAC M3
        // features.wideLines = true;
        // features.sparseResidencyBuffer = true;
        // features.sparseResidencyImage2D = true;
        // features.sparseBinding = true;

        features.shaderStorageImageWriteWithoutFormat = true;
        features.fragmentStoresAndAtomics = true;
        features.shaderInt64 = true;
        features.multiDrawIndirect = true;
        features.drawIndirectFirstInstance = true;
        features.textureCompressionBC = true;
        features.textureCompressionETC2 = true;
        features.textureCompressionASTC_LDR = true;
        // required_features.fillModeNonSolid = true; // Wireframe?

        physDeviceSelector.set_required_features(features);

        physDeviceSelector.add_required_extension("VK_KHR_timeline_semaphore");
        auto physicalDeviceResult = physDeviceSelector
                .set_surface(surface)
                .select();
        if (!physicalDeviceResult) {

            throw std::runtime_error("Failed to create physical device " + physicalDeviceResult.error().message());
        }

        physDevice = physicalDeviceResult.value();
        if (!physDevice.enable_extension_if_present("VK_KHR_timeline_semaphore")) {
            throw std::runtime_error("Failed to enable core extension");
        }
        physDevice.enable_extension_if_present(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
    }

    void VulkanContext::createPresentMode() {
        constexpr VkPresentModeKHR present_modes[] = {
            VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR,
            VK_PRESENT_MODE_FIFO_KHR
        };
        g_MainWindowData.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
            physDevice.physical_device, g_MainWindowData.Surface,
            &present_modes[0],
            IM_ARRAYSIZE(present_modes));
    }

    void VulkanContext::createSurfaceFormat() {
        g_MainWindowData.Surface = surface;
        constexpr VkFormat requestSurfaceImageFormat[] = {
            VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
            VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM
        };
        constexpr VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        g_MainWindowData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(physDevice.physical_device,
                                                                               g_MainWindowData.Surface,
                                                                               requestSurfaceImageFormat,
                                                                               IM_ARRAYSIZE(requestSurfaceImageFormat),
                                                                               requestSurfaceColorSpace);
    }

    VkBool32 VulkanContext::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                                          const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                          void *pUserData) {
        const auto severity = vkb::to_string_message_severity(messageSeverity);
        const auto type = vkb::to_string_message_type(messageType);
        printf("[%s: %s] %s\n", severity, type, pCallbackData->pMessage);
        return VK_FALSE;
    }

    void VulkanContext::addExtensions(vkb::InstanceBuilder &instanceBuilder,
                                      const ImVector<const char *> &instanceExtensions) const {
        if (auto sysInfoResult = vkb::SystemInfo::get_system_info(); sysInfoResult && debugMode) {
            const auto &sysInfo = sysInfoResult.value();
            if (sysInfo.validation_layers_available) {
                if (sysInfo.is_layer_available("VK_LAYER_LUNARG_api_dump")) {
                    instanceBuilder.enable_layer("VK_LAYER_LUNARG_api_dump");
                }
                instanceBuilder.enable_validation_layers();
                instanceBuilder.request_validation_layers();
                instanceBuilder.set_debug_callback(DebugCallback);
                instanceBuilder.set_debug_messenger_severity(
                    VK_DEBUG_REPORT_ERROR_BIT_EXT |
                    VK_DEBUG_REPORT_WARNING_BIT_EXT |
                    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT);
                instanceBuilder.set_debug_messenger_type(VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT);
            }
            if (sysInfo.is_extension_available(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
                instanceBuilder.enable_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            }
            if (sysInfo.is_extension_available(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
                instanceBuilder.enable_extension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            }
            for (const auto &extension: instanceExtensions) {
                instanceBuilder.enable_extension(extension);
            }
        }
    }

    void VulkanContext::createDescriptorPool() {
        constexpr VkDescriptorPoolSize pool_sizes[] =
        {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1;
        pool_info.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes));
        pool_info.pPoolSizes = pool_sizes;
        VulkanUtils::CheckVKResult(vkCreateDescriptorPool(device.device, &pool_info, instance.allocation_callbacks,
                                                          &descriptorPool));
    }

    void VulkanContext::setupVulkan(GLFWwindow *w, const ImVector<const char *> &instanceExtensions) {
        this->window = w;
        vkb::InstanceBuilder instanceBuilder;

        // ----- INSTANCE AND EXTENSIONS
        addExtensions(instanceBuilder, instanceExtensions);
        auto vkbResult = instanceBuilder
                .set_app_name(ENGINE_NAME)
                .set_engine_name(ENGINE_NAME)
                .require_api_version(1, 0, 0)
                .build();
        if (!vkbResult) {
            throw std::runtime_error("Failed to create vulkan instance.");
        }
        instance = vkbResult.value();
        VulkanUtils::CheckVKResult(
            glfwCreateWindowSurface(instance.instance, window, instance.allocation_callbacks, &surface));

        createPhysicalDevice();
        createDevice();
        createQueue();
        createDescriptorPool();
        createSurfaceFormat();
        createPresentMode();
        createSwapChain();
    }
}
