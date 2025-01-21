#ifndef METAL_ENGINE_VULKANCONTEXT_H
#define METAL_ENGINE_VULKANCONTEXT_H

#include "vk_mem_alloc.h"
#include <unordered_map>

#include "imgui_impl_vulkan.h"
#include <GLFW/glfw3.h>
#include "VkBootstrap.h"
#include "../../common/AbstractRuntimeComponent.h"

#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

namespace Metal {
    class VulkanContext final : public AbstractRuntimeComponent {
        static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                      void *pUserData);

        void addExtensions(vkb::InstanceBuilder &instanceBuilder,
                           const ImVector<const char *> &instanceExtensions) const;

        void createSwapChain();

        void createQueue();

        void createDevice();

        void createPhysicalDevice();

        void createPresentMode();

        void createSurfaceFormat();

        void createMemoryAllocator();

        void createCommandPool();

        void createDescriptorPool() const;

        uint32_t w{}, h{};
        GLFWwindow *window = nullptr;
        bool debugMode = false;
        std::vector<VkCommandBuffer> commandBuffers{};

    public:
        void pushCommandBuffer(VkCommandBuffer commandBuffer) {
            commandBuffers.push_back(commandBuffer);
        }

        std::vector<VkCommandBuffer> &getCommandBuffers() { return commandBuffers; }

        explicit VulkanContext(ApplicationContext &context, bool debugMode);

        VkPhysicalDeviceProperties physicalDeviceProperties{};
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};
        vkb::Instance instance{};
        ImGui_ImplVulkanH_Window imguiVulkanWindow;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        uint32_t queueFamily{};
        vkb::PhysicalDevice physDevice;
        vkb::Device device;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        VmaAllocator allocator = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;

        void dispose() const;

        void onInitialize() override;

        [[nodiscard]] uint32_t getWindowHeight() const {
            return h;
        }

        [[nodiscard]] uint32_t getWindowWidth() const {
            return w;
        }

        [[nodiscard]] VkCommandBuffer beginSingleTimeCommands() const;

        void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

        void submitFrame(VkSemaphore image_acquired_semaphore, VkSemaphore render_complete_semaphore,
                         ImGui_ImplVulkanH_Frame *fd) const;
    };
} // Metal

#endif
