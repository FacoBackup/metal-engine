#ifndef METAL_ENGINE_VULKANCONTEXT_H
#define METAL_ENGINE_VULKANCONTEXT_H


#include "vk_mem_alloc.h"

#include "imgui_impl_vulkan.h"
#include <GLFW/glfw3.h>
#include "VkBootstrap.h"
#include "VulkanFrameData.h"

#define IMAGE_COUNT 2
#define MAX_DESCRIPTOR_SETS 1
#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

namespace Metal {
    class VulkanContext {
        static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                      void *pUserData);

        void addExtensions(vkb::InstanceBuilder &instanceBuilder,
                           const ImVector<const char *> &instanceExtensions) const;

        void createDescriptorPool();

        void createSwapChain();

        void createQueue();

        void createDevice();

        void createPhysicalDevice();

        void createPresentMode();

        void createSurfaceFormat();

        void createMemoryAllocator();

        uint32_t w{}, h{};
        GLFWwindow *window = nullptr;
        bool debugMode = false;
        VulkanFrameData frameData{};

    public:
        vkb::Instance instance{};
        ImGui_ImplVulkanH_Window imguiVulkanWindow;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        uint32_t queueFamily{};
        vkb::PhysicalDevice physDevice;
        vkb::Device device;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
        VmaAllocator allocator = VK_NULL_HANDLE;

        void build(bool debugMode);

        VulkanFrameData &getFrameData();

        void dispose() const;

        void setupVulkan(GLFWwindow *window, const ImVector<const char *> &instanceExtensions);

        [[nodiscard]] uint32_t getWindowWidth() const {
            return w;
        }

        [[nodiscard]] uint32_t getWindowHeight() const {
            return h;
        }
    };
} // Metal

#endif
