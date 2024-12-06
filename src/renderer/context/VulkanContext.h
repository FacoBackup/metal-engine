#ifndef METAL_ENGINE_VULKANCONTEXT_H
#define METAL_ENGINE_VULKANCONTEXT_H

#include "imgui_impl_vulkan.h"
#include <GLFW/glfw3.h>
#include "VkBootstrap.h"

#define MIN_IMAGE_COUNT 2

namespace Metal {
    class VulkanContext {
        void frameRender(ImDrawData *draw_data);

        static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                      void *pUserData);

        void addExtensions(vkb::InstanceBuilder &instanceBuilder,
                           const ImVector<const char *> &instanceExtensions) const;

        void createDescriptorPool();

        int w{}, h{};
        GLFWwindow *window = nullptr;

    public:
        bool debugMode = false;
        bool isSwapChainRebuild = false;
        vkb::Instance instance{};
        ImGui_ImplVulkanH_Window g_MainWindowData;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        uint32_t queueFamily{};
        vkb::PhysicalDevice physDevice;
        vkb::Device device;
        VkQueue graphicsQueue{};
        VkSwapchainKHR swapChain{};
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

        void build(bool debugMode);

        void shutdown() const;

        void createSwapChain();

        void createQueue();

        void createDevice();

        void createPhysicalDevice();

        void createPresentMode();

        void createSurfaceFormat();

        void setupVulkan(GLFWwindow *window, const ImVector<const char *> &instanceExtensions);
    };
} // Metal

#endif
