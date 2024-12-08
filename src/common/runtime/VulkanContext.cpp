#include "VulkanContext.h"

#include "VkBootstrap.h"
#include "../util/VulkanUtils.h"
#include "ApplicationContext.h"

namespace Metal {
    void VulkanContext::build(const bool d) {
        this->debugMode = d;
        imguiVulkanWindow.ClearValue.color.float32[0] = 0;
        imguiVulkanWindow.ClearValue.color.float32[1] = 0;
        imguiVulkanWindow.ClearValue.color.float32[2] = 0;
        imguiVulkanWindow.ClearValue.color.float32[3] = 0;
    }

    VulkanFrameData &VulkanContext::getFrameData() {
        auto [CommandPool, CommandBuffer, Fence, Backbuffer, BackbufferView, Framebuffer] = imguiVulkanWindow.Frames[
            imguiVulkanWindow.FrameIndex];
        frameData.commandPool = CommandPool;
        frameData.commandBuffer = CommandBuffer;
        frameData.fence = Fence;
        frameData.backbuffer = Backbuffer;
        frameData.backbufferView = BackbufferView;
        frameData.framebuffer = Framebuffer;
        frameData.imageAcquiredSemaphore = imguiVulkanWindow.FrameSemaphores[imguiVulkanWindow.SemaphoreIndex].
                ImageAcquiredSemaphore;
        frameData.renderCompleteSemaphore = imguiVulkanWindow.FrameSemaphores[imguiVulkanWindow.SemaphoreIndex].
                RenderCompleteSemaphore;
        frameData.frameIndex = imguiVulkanWindow.FrameIndex;
        return frameData;
    }

    void VulkanContext::createSwapChain() {
        int w{}, h{};
        glfwGetFramebufferSize(window, &w, &h);
        ImGui_ImplVulkanH_CreateOrResizeWindow(instance.instance, physDevice.physical_device,
                                               device.device, &imguiVulkanWindow, queueFamily,
                                               instance.allocation_callbacks,
                                               w, h, IMAGE_COUNT);
        swapChain = imguiVulkanWindow.Swapchain;

        int wW{}, hW{};
        glfwGetWindowSize(window, &wW, &hW);
        this->w = wW;
        this->h = hW;
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
        imguiVulkanWindow.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
            physDevice.physical_device, imguiVulkanWindow.Surface,
            &present_modes[0],
            IM_ARRAYSIZE(present_modes));
    }

    void VulkanContext::createSurfaceFormat() {
        imguiVulkanWindow.Surface = surface;
        constexpr VkFormat requestSurfaceImageFormat[] = {
            VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
            VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM
        };
        constexpr VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        imguiVulkanWindow.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(physDevice.physical_device,
            imguiVulkanWindow.Surface,
            requestSurfaceImageFormat,
            IM_ARRAYSIZE(requestSurfaceImageFormat),
            requestSurfaceColorSpace);
    }

    void VulkanContext::createMemoryAllocator() {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = physDevice.physical_device;
        allocatorInfo.device = device.device;
        allocatorInfo.instance = instance.instance;
        vmaCreateAllocator(&allocatorInfo, &allocator);
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
                // if (sysInfo.is_layer_available("VK_LAYER_LUNARG_api_dump")) {
                //     instanceBuilder.enable_layer("VK_LAYER_LUNARG_api_dump");
                // }
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
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_DESCRIPTOR_SETS},
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = MAX_DESCRIPTOR_SETS;
        pool_info.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes));
        pool_info.pPoolSizes = pool_sizes;
        VulkanUtils::CheckVKResult(vkCreateDescriptorPool(device.device, &pool_info, instance.allocation_callbacks,
                                                          &imguiDescriptorPool));
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
        createMemoryAllocator();
    }

    void VulkanContext::dispose() const {
        vkDestroyDescriptorPool(device.device, imguiDescriptorPool, instance.allocation_callbacks);
        vkb::destroy_device(device);
        vkb::destroy_instance(instance);
    }
}
