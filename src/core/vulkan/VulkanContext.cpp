#include "VulkanContext.h"
#include "VkBootstrap.h"
#include "VulkanUtils.h"
#include "../../ApplicationContext.h"
#include "../../common/LoggerUtil.h"

#include "../glfw/GLFWContext.h"
#include "../../engine/repository/EngineRepository.h"
#include "../../engine/service/MeshService.h"
#include "../../engine/service/TextureService.h"
#include "../../engine/service/FrameBufferService.h"
#include "../../engine/service/PipelineService.h"
#include "../../engine/service/RayTracingService.h"

namespace Metal {

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
        this->w = static_cast<unsigned int>(wW);
        this->h = static_cast<unsigned int>(hW);
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
        for (unsigned int i = 0; i < static_cast<unsigned int>(queue_families.size()); i++) {
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
        vkb::DeviceBuilder deviceBuilder{physDevice};

        VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{};
        bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
        bufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelFeatures{};
        accelFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        accelFeatures.accelerationStructure = VK_TRUE;

        VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeatures{};
        rtPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        rtPipelineFeatures.rayTracingPipeline = VK_TRUE;

        deviceBuilder.add_pNext(&bufferDeviceAddressFeatures);
        deviceBuilder.add_pNext(&accelFeatures);
        deviceBuilder.add_pNext(&rtPipelineFeatures);

        auto deviceResult = deviceBuilder.build();
        if (!deviceResult) {
            throw std::runtime_error("Failed to create device.");
        }
        device = deviceResult.value();

        vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(
            vkGetDeviceProcAddr(device.device, "vkGetBufferDeviceAddressKHR"));
        vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(
            vkGetDeviceProcAddr(device.device, "vkCreateAccelerationStructureKHR"));
        vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(
            vkGetDeviceProcAddr(device.device, "vkDestroyAccelerationStructureKHR"));
        vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(
            vkGetDeviceProcAddr(device.device, "vkGetAccelerationStructureBuildSizesKHR"));
        vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(
            vkGetDeviceProcAddr(device.device, "vkCmdBuildAccelerationStructuresKHR"));
        vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(
            vkGetDeviceProcAddr(device.device, "vkGetAccelerationStructureDeviceAddressKHR"));
        vkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(
            vkGetDeviceProcAddr(device.device, "vkCreateRayTracingPipelinesKHR"));
        vkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(
            vkGetDeviceProcAddr(device.device, "vkGetRayTracingShaderGroupHandlesKHR"));
        vkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(
            vkGetDeviceProcAddr(device.device, "vkCmdTraceRaysKHR"));

        rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
        VkPhysicalDeviceProperties2 deviceProperties2{};
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.pNext = &rayTracingPipelineProperties;
        vkGetPhysicalDeviceProperties2(physDevice.physical_device, &deviceProperties2);
    }

    void VulkanContext::createPhysicalDevice() {
        vkb::PhysicalDeviceSelector physDeviceSelector(instance);

        auto physicalDeviceResult = physDeviceSelector
                .set_surface(surface)
                .require_present()
                .select();

        if (!physicalDeviceResult) {
            throw std::runtime_error("Failed to create physical device " + physicalDeviceResult.error().message());
        }

        physDevice = physicalDeviceResult.value();

        VkPhysicalDeviceFeatures features{};
        features.tessellationShader = VK_TRUE;
        features.shaderStorageImageWriteWithoutFormat = VK_TRUE;
        features.fragmentStoresAndAtomics = VK_TRUE;
        features.shaderInt64 = VK_TRUE;
        features.multiDrawIndirect = VK_TRUE;
        features.drawIndirectFirstInstance = VK_TRUE;

        // Check if features are supported before requesting
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(physDevice.physical_device, &supportedFeatures);

        if (!supportedFeatures.tessellationShader) features.tessellationShader = VK_FALSE;
        if (!supportedFeatures.shaderStorageImageWriteWithoutFormat)
            features.shaderStorageImageWriteWithoutFormat = VK_FALSE;
        if (!supportedFeatures.fragmentStoresAndAtomics) features.fragmentStoresAndAtomics = VK_FALSE;
        if (!supportedFeatures.shaderInt64) features.shaderInt64 = VK_FALSE;
        if (!supportedFeatures.multiDrawIndirect) features.multiDrawIndirect = VK_FALSE;
        if (!supportedFeatures.drawIndirectFirstInstance) features.drawIndirectFirstInstance = VK_FALSE;

        physDeviceSelector.set_required_features(features);
        physicalDeviceResult = physDeviceSelector.select();
        if (physicalDeviceResult) {
            physDevice = physicalDeviceResult.value();
        }

        vkGetPhysicalDeviceProperties(physDevice.physical_device, &physicalDeviceProperties);
        LOG_INFO(
            "MAX SAMPLERS " + std::to_string(physicalDeviceProperties.limits.maxDescriptorSetSamplers) + " " + std::
            to_string(physicalDeviceProperties.limits.maxPerStageDescriptorSampledImages));
        vkGetPhysicalDeviceMemoryProperties(physDevice.physical_device, &physicalDeviceMemoryProperties);
        if (!physDevice.enable_extension_if_present("VK_KHR_timeline_semaphore")) {
            throw std::runtime_error("Failed to enable core extension");
        }
        physDevice.enable_extension_if_present(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
        physDevice.enable_extension_if_present(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        // Ray tracing extensions
        bool rayTracingSupported =
                physDevice.enable_extension_if_present(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME) &&
                physDevice.enable_extension_if_present(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) &&
                physDevice.enable_extension_if_present(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME) &&
                physDevice.enable_extension_if_present(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);

        if (rayTracingSupported) {
            LOG_INFO("Ray tracing extensions enabled");
        } else {
            throw std::runtime_error("Ray tracing extensions NOT supported");
        }
    }

    void VulkanContext::createPresentMode() {
        VkPresentModeKHR presentModes = !engineRepository->vsync
                                            ? VK_PRESENT_MODE_IMMEDIATE_KHR
                                            : VK_PRESENT_MODE_FIFO_KHR;
        imguiVulkanWindow.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
            physDevice.physical_device, imguiVulkanWindow.Surface,
            &presentModes,
            1);
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
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
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
        this->debugMode = CTX->isDebugMode();
        this->window = glfwContext->getWindow();
        
        imguiVulkanWindow.ClearValue.color.float32[0] = 0;
        imguiVulkanWindow.ClearValue.color.float32[1] = 0;
        imguiVulkanWindow.ClearValue.color.float32[2] = 0;
        imguiVulkanWindow.ClearValue.color.float32[3] = 1;
        vkb::InstanceBuilder instanceBuilder;

        // ------- CORE INITIALIZATION
        // ----- INSTANCE AND EXTENSIONS
        addExtensions(instanceBuilder, glfwContext->getInstanceExtensions());
        auto vkbResult = instanceBuilder
                .set_app_name(ENGINE_NAME)
                .set_engine_name(ENGINE_NAME)
                .require_api_version(1, 2, 0)
                .enable_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)
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
        textureService->createSampler(false, vkImageSampler, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
        textureService->createSampler(true, vkTextureSampler, VK_SAMPLER_ADDRESS_MODE_REPEAT);
        // ------- CORE INITIALIZATION
    }

    void VulkanContext::dispose() {
        pipelineService->disposeAll();
        textureService->disposeAll();
        meshService->disposeAll();
        framebufferService->disposeAll();
        rayTracingService->dispose();

        vkDestroySampler(device.device, vkImageSampler, nullptr);
        vkDestroySampler(device.device, vkTextureSampler, nullptr);

        vkDestroyDescriptorPool(device.device, descriptorPool,
                                nullptr);
        vkDestroyCommandPool(device.device, commandPool,
                             nullptr);

        vkDestroyDevice(device.device, nullptr);
        vkDestroySurfaceKHR(instance.instance, surface, nullptr);
        vkb::destroy_instance(instance);
    }

    void VulkanContext::createDescriptorPool() const {
        const std::array<VkDescriptorPoolSize, 5> sizes{
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100}, // 1 for imgui
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100},
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100},
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 10}
        };

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
        poolInfo.pPoolSizes = sizes.data();
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT |
                         VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
        poolInfo.maxSets = 500;

        VulkanUtils::CheckVKResult(vkCreateDescriptorPool(device.device, &poolInfo,
                                                          nullptr, const_cast<VkDescriptorPool*>(&descriptorPool)));
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
                                    ImGui_ImplVulkanH_Frame *fd) {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = {};
        pushCommandBuffer(fd->CommandBuffer);
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &image_acquired_semaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = getCommandBuffers().size();
        info.pCommandBuffers = getCommandBuffers().data();
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &render_complete_semaphore;
        VulkanUtils::CheckVKResult(vkEndCommandBuffer(fd->CommandBuffer));
        VulkanUtils::CheckVKResult(vkQueueSubmit(graphicsQueue, 1, &info, fd->Fence));
    }
}
