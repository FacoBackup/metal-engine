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
                                               w, h, MAX_FRAMES_IN_FLIGHT);
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
        features.fragmentStoresAndAtomics = true;
        features.shaderInt64 = true;
        features.multiDrawIndirect = true;
        features.drawIndirectFirstInstance = true;
        features.textureCompressionBC = true;
        features.textureCompressionETC2 = true;
        features.textureCompressionASTC_LDR = true;
        // required_features.fillModeNonSolid = true; // Wireframe?

        physDeviceSelector.set_required_features(features);

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
        physDevice.enable_extension_if_present(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
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
                    VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                    VK_DEBUG_REPORT_WARNING_BIT_EXT |
                    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                    VK_DEBUG_REPORT_ERROR_BIT_EXT |
                    VK_DEBUG_REPORT_DEBUG_BIT_EXT |
                    VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT
                );
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
        cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
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
        createDescriptorPool();
        // ------- CORE INITIALIZATION

        // ------- REPOSITORY INITIALIZATION
        coreBuffers.onInitialize();
        coreFrameBuffers.onInitialize();
        coreTextures.onInitialize();
        coreDescriptorSets.onInitialize();
        corePipelines.onInitialize();
        coreRenderPasses.onInitialize();
        // ------- REPOSITORY INITIALIZATION
    }

    void VulkanContext::dispose() {
        pipelineService.disposeAll();
        textureService.disposeAll();
        meshService.disposeAll();
        framebufferService.disposeAll();
        vkDestroyDescriptorPool(context.vulkanContext.device.device, descriptorPool,
                                nullptr);
        vkDestroyCommandPool(context.vulkanContext.device.device, commandPool,
                             nullptr);

        vkDestroyDevice(device.device, nullptr);
        vkDestroySurfaceKHR(instance.instance, surface, nullptr);
        vkb::destroy_instance(instance);
    }

    void VulkanContext::createDescriptorPool() const {
        const std::array sizes{
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100}, // 1 for imgui
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
        };

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = sizes.size();
        poolInfo.pPoolSizes = sizes.data();
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 101;

        VulkanUtils::CheckVKResult(vkCreateDescriptorPool(device.device, &poolInfo,
                                                          nullptr, &context.vulkanContext.descriptorPool));
    }

    VkCommandBuffer VulkanContext::beginSingleTimeCommands() const {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device.device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void VulkanContext::endSingleTimeCommands(VkCommandBuffer commandBuffer) const {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device.device, commandPool, 1, &commandBuffer);
    }


    void VulkanContext::submitFrame(VkSemaphore image_acquired_semaphore, VkSemaphore render_complete_semaphore,
                                    ImGui_ImplVulkanH_Frame *fd) const {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = {};
        context.vulkanContext.pushCommandBuffer(fd->CommandBuffer);
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &image_acquired_semaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = context.vulkanContext.getCommandBuffers().size();
        info.pCommandBuffers = context.vulkanContext.getCommandBuffers().data();
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &render_complete_semaphore;
        VulkanUtils::CheckVKResult(vkEndCommandBuffer(fd->CommandBuffer));
        VulkanUtils::CheckVKResult(vkQueueSubmit(context.vulkanContext.graphicsQueue, 1, &info, fd->Fence));
    }
}
