#ifndef METAL_ENGINE_VULKANCONTEXT_H
#define METAL_ENGINE_VULKANCONTEXT_H

#include "imgui_impl_vulkan.h"
#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>


namespace Metal {

    class VulkanContext {
    public:
        void build(bool debugMode);

        bool debugMode = false;
        int g_MinImageCount = 2;
        bool g_SwapChainRebuild = false;
        VkAllocationCallbacks *g_Allocator = nullptr;
        VkInstance g_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice g_Device = VK_NULL_HANDLE;
        uint32_t g_QueueFamily = (uint32_t) -1;
        VkQueue g_Queue = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
        VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
        VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
        ImGui_ImplVulkanH_Window g_MainWindowData;
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        void shutdown();

        void renderFrame(ImDrawData *main_draw_data);

        void setupVulkan();

        void setupVulkanWindow(GLFWwindow *window);

    private:

        void frameRender(ImDrawData *draw_data);

        void createSurface(GLFWwindow *window);
    };

} // Metal

#endif
