#include "VulkanContext.h"

#include "VkBootstrap.h"
#include "../common/util/VulkanUtils.h"
#include "ApplicationContext.h"

namespace Metal {
    VulkanContext::VulkanContext(ApplicationContext &context, bool debugMode) : AbstractRuntimeComponent(context),
        debugMode(debugMode) {
    }

    void VulkanContext::createSwapChain() {
        int w{}, h{};
        glfwGetFramebufferSize(window, &w, &h);
        ImGui_ImplVulkanH_CreateOrResizeWindow(instance.instance, physDevice.physical_device,
                                               device.device, &imguiVulkanWindow, queueFamily,
                                               nullptr,
                                               w, h, IMAGE_COUNT);
        swapChain = imguiVulkanWindow.Swapchain;

        int wW{}, hW{};
        glfwGetWindowSize(window, &wW, &hW);
        this->w = static_cast<uint32_t>(wW);
        this->h = static_cast<uint32_t>(hW);
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
        vkGetPhysicalDeviceProperties(physDevice.physical_device, &physicalDeviceProperties);
        vkGetPhysicalDeviceMemoryProperties(physDevice.physical_device, &physicalDeviceMemoryProperties);
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
        if (auto sysInfoResult = vkb::SystemInfo::get_system_info(); sysInfoResult) {
            const auto &sysInfo = sysInfoResult.value();
            if (sysInfo.validation_layers_available && debugMode) {
                if (sysInfo.is_layer_available("VK_LAYER_LUNARG_api_dump")) {
                    // instanceBuilder.enable_layer("VK_LAYER_LUNARG_api_dump");
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

    void VulkanContext::createCommandPool() {
        VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
        cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCreateInfo.pNext = nullptr;
        cmdPoolCreateInfo.flags = 0;
        cmdPoolCreateInfo.queueFamilyIndex = queueFamily;
        VulkanUtils::CheckVKResult(vkCreateCommandPool(device.device, &cmdPoolCreateInfo,
                                                       nullptr,
                                                       &commandPool));
    }

    void VulkanContext::onInitialize() {
        imguiVulkanWindow.ClearValue.color.float32[0] = 0;
        imguiVulkanWindow.ClearValue.color.float32[1] = 0;
        imguiVulkanWindow.ClearValue.color.float32[2] = 0;
        imguiVulkanWindow.ClearValue.color.float32[3] = 0;

        this->window = context.getGLFWContext().getWindow();
        vkb::InstanceBuilder instanceBuilder;

        // ------- CORE INITIALIZATION
        // ----- INSTANCE AND EXTENSIONS
        addExtensions(instanceBuilder, context.getGLFWContext().getInstanceExtensions());
        auto vkbResult = instanceBuilder
                .set_app_name(ENGINE_NAME)
                .set_engine_name(ENGINE_NAME)
                .require_api_version(1, 0, 0)
                .build();
        if (!vkbResult) {
            throw std::runtime_error("Failed to create runtime instance.");
        }
        instance = vkbResult.value();
        VulkanUtils::CheckVKResult(
            glfwCreateWindowSurface(instance.instance, window, nullptr, &surface));
        createPhysicalDevice();
        createDevice();
        createQueue();
        createSurfaceFormat();
        createPresentMode();
        createSwapChain();
        createMemoryAllocator();
        createCommandPool();
        // ------- CORE INITIALIZATION


        // ------- SERVICE INITIALIZATION
        textureService.onInitialize();
        framebufferService.onInitialize();
        pipelineService.onInitialize();
        shaderService.onInitialize();
        descriptorService.onInitialize();
        commandService.onInitialize();
        // ------- SERVICE INITIALIZATION

        // ------- REPOSITORY INITIALIZATION
        coreBuffers.onInitialize();
        coreFrameBuffers.onInitialize();
        coreDescriptorSets.onInitialize();
        createDescriptorPool();
        coreDescriptorSets.createDescriptors();
        corePipelines.onInitialize();
        // ------- REPOSITORY INITIALIZATION
    }

    void VulkanContext::dispose() const {
        vkDestroyDescriptorPool(context.getVulkanContext().device.device, descriptorPool,
                                nullptr);
        vkDestroyCommandPool(context.getVulkanContext().device.device, commandPool,
                             nullptr);

        vkb::destroy_device(device);
        vkb::destroy_instance(instance);
    }

    void VulkanContext::createDescriptorPool() {
        // IMGUI REQUIREMENT
        registerDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        registerDescriptorSet();
        // IMGUI REQUIREMENT

        std::vector<VkDescriptorPoolSize> sizes{};
        sizes.reserve(descriptorPoolSizes.size());
        for (const auto &item: descriptorPoolSizes) {
            sizes.emplace_back(item.first, item.second);
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = sizes.size();
        poolInfo.pPoolSizes = sizes.data();
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = maxDescriptorSets;

        VulkanUtils::CheckVKResult(vkCreateDescriptorPool(device.device, &poolInfo,
                                                          nullptr, &context.getVulkanContext().descriptorPool));
    }

    void VulkanContext::registerDescriptorBinding(VkDescriptorType type) {
        if (!descriptorPoolSizes.contains(type)) {
            descriptorPoolSizes.insert({type, 0});
        }
        descriptorPoolSizes.insert({type, descriptorPoolSizes[type]++});
    }

    void VulkanContext::registerDescriptorSet() {
        maxDescriptorSets++;
    }

    uint32_t VulkanContext::getWindowHeight() const {
        return h;
    }

    uint32_t VulkanContext::getWindowWidth() const {
        return w;
    }

    FrameData &VulkanContext::getFrameData() {
        return frameData;
    }
}
