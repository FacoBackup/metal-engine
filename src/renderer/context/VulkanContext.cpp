#include "VulkanContext.h"
#include "../VulkanUtils.h"

#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

namespace Metal {
    void VulkanContext::shutdown() {
        vkDestroyDescriptorPool(g_Device, g_DescriptorPool,
                                g_Allocator);

        if (debugMode) {
            // Remove the debug report callback
            auto f_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(
                    g_Instance, "vkDestroyDebugReportCallbackEXT");
            f_vkDestroyDebugReportCallbackEXT(g_Instance, g_DebugReport, g_Allocator);
        }

        vkDestroyDevice(g_Device, g_Allocator);
        vkDestroyInstance(g_Instance, g_Allocator);
    }

    void VulkanContext::build(bool debugMode) {
        this->debugMode = debugMode;
        g_MainWindowData.ClearValue.color.float32[0] = 0;
        g_MainWindowData.ClearValue.color.float32[1] = 0;
        g_MainWindowData.ClearValue.color.float32[2] = 0;
        g_MainWindowData.ClearValue.color.float32[3] = 0;
    }

    void VulkanContext::frameRender(ImDrawData *draw_data) {
        VkResult err;

        VkSemaphore image_acquired_semaphore = g_MainWindowData.FrameSemaphores[g_MainWindowData.SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore render_complete_semaphore = g_MainWindowData.FrameSemaphores[g_MainWindowData.SemaphoreIndex].RenderCompleteSemaphore;
        err = vkAcquireNextImageKHR(g_Device, g_MainWindowData.Swapchain, UINT64_MAX, image_acquired_semaphore,
                                    VK_NULL_HANDLE,
                                    &g_MainWindowData.FrameIndex);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            g_SwapChainRebuild = true;
            return;
        }
        Metal::VulkanUtils::check_vk_result(err);

        ImGui_ImplVulkanH_Frame *fd = &g_MainWindowData.Frames[g_MainWindowData.FrameIndex];
        {
            err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE,
                                  UINT64_MAX);    // wait indefinitely instead of periodically checking
            Metal::VulkanUtils::check_vk_result(err);

            err = vkResetFences(g_Device, 1, &fd->Fence);
            Metal::VulkanUtils::check_vk_result(err);
        }
        {
            err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
            Metal::VulkanUtils::check_vk_result(err);
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
            Metal::VulkanUtils::check_vk_result(err);
        }
        {
            VkRenderPassBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass = g_MainWindowData.RenderPass;
            info.framebuffer = fd->Framebuffer;
            info.renderArea.extent.width = g_MainWindowData.Width;
            info.renderArea.extent.height = g_MainWindowData.Height;
            info.clearValueCount = 1;
            info.pClearValues = &g_MainWindowData.ClearValue;
            vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

        // Submit command buffer
        vkCmdEndRenderPass(fd->CommandBuffer);
        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount = 1;
            info.pWaitSemaphores = &image_acquired_semaphore;
            info.pWaitDstStageMask = &wait_stage;
            info.commandBufferCount = 1;
            info.pCommandBuffers = &fd->CommandBuffer;
            info.signalSemaphoreCount = 1;
            info.pSignalSemaphores = &render_complete_semaphore;

            err = vkEndCommandBuffer(fd->CommandBuffer);
            Metal::VulkanUtils::check_vk_result(err);
            err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
            Metal::VulkanUtils::check_vk_result(err);
        }
    }

    void VulkanContext::renderFrame(ImDrawData *main_draw_data) {
        frameRender(main_draw_data);
        Metal::VulkanUtils::FramePresent(&g_MainWindowData, g_Queue, g_SwapChainRebuild);

    }

    void VulkanContext::setupVulkan() {
        ImVector<const char *> instance_extensions;
        uint32_t extensions_count = 0;
        const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
        for (uint32_t i = 0; i < extensions_count; i++)
            instance_extensions.push_back(glfw_extensions[i]);

        VkResult err;

        // Create Vulkan Instance
        {
            VkInstanceCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

            // Enumerate available extensions
            uint32_t properties_count;
            ImVector<VkExtensionProperties> properties;
            vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
            properties.resize(properties_count);
            err = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.Data);
            Metal::VulkanUtils::check_vk_result(err);

            // Enable required extensions
            if (Metal::VulkanUtils::IsExtensionAvailable(properties,VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
                instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

            #ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
            if (Metal::VulkanUtils::IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
                instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
                create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
            }
            #endif

            // Enabling validation layers
            if (debugMode) {
                const char *layers[] = {"VK_LAYER_KHRONOS_validation"};
                create_info.enabledLayerCount = 1;
                create_info.ppEnabledLayerNames = layers;
                instance_extensions.push_back("VK_EXT_debug_report");
            }

            // Create Vulkan Instance
            create_info.enabledExtensionCount = (uint32_t) instance_extensions.Size;
            create_info.ppEnabledExtensionNames = instance_extensions.Data;
            err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
            Metal::VulkanUtils::check_vk_result(err);


            // Setup the debug report callback
            if (debugMode) {
                auto f_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(
                        g_Instance, "vkCreateDebugReportCallbackEXT");
                IM_ASSERT(f_vkCreateDebugReportCallbackEXT != nullptr);
                VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
                debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
                debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
                debug_report_ci.pfnCallback = Metal::VulkanUtils::debug_report;
                debug_report_ci.pUserData = nullptr;
                err = f_vkCreateDebugReportCallbackEXT(g_Instance, &debug_report_ci,
                                                       g_Allocator, &g_DebugReport);
                Metal::VulkanUtils::check_vk_result(err);
            }
        }

        // Select Physical Device (GPU)
        g_PhysicalDevice = Metal::VulkanUtils::SelectPhysicalDevice(*this);

        // Select graphics queue family
        {
            uint32_t count;
            vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, nullptr);
            VkQueueFamilyProperties *queues = (VkQueueFamilyProperties *) malloc(
                    sizeof(VkQueueFamilyProperties) * count);
            vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);
            for (uint32_t i = 0; i < count; i++)
                if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    g_QueueFamily = i;
                    break;
                }
            free(queues);
            IM_ASSERT(g_QueueFamily != (uint32_t) -1);
        }

        // Create Logical Device (with 1 queue)
        {
            ImVector<const char *> device_extensions;
            device_extensions.push_back("VK_KHR_swapchain");

            // Enumerate physical device extension
            uint32_t properties_count;
            ImVector<VkExtensionProperties> properties;
            vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count,
                                                 nullptr);
            properties.resize(properties_count);
            vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count,
                                                 properties.Data);
            #ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
            if (Metal::VulkanUtils::IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
            device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
            #endif

            const float queue_priority[] = {1.0f};
            VkDeviceQueueCreateInfo queue_info[1] = {};
            queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info[0].queueFamilyIndex = g_QueueFamily;
            queue_info[0].queueCount = 1;
            queue_info[0].pQueuePriorities = queue_priority;
            VkDeviceCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
            create_info.pQueueCreateInfos = queue_info;
            create_info.enabledExtensionCount = (uint32_t) device_extensions.Size;
            create_info.ppEnabledExtensionNames = device_extensions.Data;
            err = vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator,
                                 &g_Device);
            Metal::VulkanUtils::check_vk_result(err);
            vkGetDeviceQueue(g_Device, g_QueueFamily, 0,
                             &g_Queue);
        }

        // Create Descriptor Pool
        // The example only requires a single combined image sampler descriptor for the font image and only uses one descriptor set (for that)
        // If you wish to load e.g. additional textures you may need to alter pools sizes.
        {
            VkDescriptorPoolSize pool_sizes[] =
                    {
                            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
                    };
            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 1;
            pool_info.poolSizeCount = (uint32_t) IM_ARRAYSIZE(pool_sizes);
            pool_info.pPoolSizes = pool_sizes;
            err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator,
                                         &g_DescriptorPool);
            Metal::VulkanUtils::check_vk_result(err);
        }
    }

    void VulkanContext::setupVulkanWindow(GLFWwindow *window) {
        createSurface(window);

        // Create Framebuffers
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);


        // Setup Window
        g_MainWindowData.Surface = surface;

        // Check for WSI support
        VkBool32 res;
        vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, g_MainWindowData.Surface, &res);
        if (res != VK_TRUE) {
            fprintf(stderr, "Error no WSI support on physical device 0\n");
            exit(-1);
        }

        // Select Surface Format
        const VkFormat requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
                                                      VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
        const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        g_MainWindowData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice,
                                                                               g_MainWindowData.Surface,
                                                                               requestSurfaceImageFormat,
                                                                               (size_t) IM_ARRAYSIZE(
                                                                                       requestSurfaceImageFormat),
                                                                               requestSurfaceColorSpace);


        VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR,
                                            VK_PRESENT_MODE_FIFO_KHR};
        g_MainWindowData.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, g_MainWindowData.Surface,
                                                                           &present_modes[0],
                                                                           IM_ARRAYSIZE(present_modes));

        // Create SwapChain, RenderPass, Framebuffer, etc.
        IM_ASSERT(g_MinImageCount >= 2);
        ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice,
                                               g_Device, &g_MainWindowData, g_QueueFamily,
                                               g_Allocator,
                                               w, h, g_MinImageCount);
    }

    void VulkanContext::createSurface(GLFWwindow *window) {
        VkResult err = glfwCreateWindowSurface(g_Instance, window, g_Allocator, &surface);
        Metal::VulkanUtils::check_vk_result(err);
    }
}