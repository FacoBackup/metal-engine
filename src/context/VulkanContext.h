#ifndef METAL_ENGINE_VULKANCONTEXT_H
#define METAL_ENGINE_VULKANCONTEXT_H


#include "vk_mem_alloc.h"
#include <unordered_map>

#include "imgui_impl_vulkan.h"
#include <GLFW/glfw3.h>
#include "VkBootstrap.h"
#include "repository/CoreFrameBuffers.h"
#include "repository/CorePipelines.h"
#include "repository/CoreBuffers.h"
#include "repository/CoreDescriptorSets.h"
#include "../common/interface/AbstractRuntimeComponent.h"
#include "runtime/FrameData.h"
#include "service/DescriptorService.h"
#include "service/TextureService.h"
#include "service/FrameBufferService.h"
#include "service/PipelineService.h"
#include "service/ShaderService.h"
#include "service/CommandService.h"
#include "service/BufferService.h"

#define IMAGE_COUNT 2
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

        void createDescriptorPool();

        uint32_t w{}, h{};
        GLFWwindow *window = nullptr;
        bool debugMode = false;
        std::unordered_map<VkDescriptorType, uint32_t> descriptorPoolSizes{};
        FrameData frameData{};
        uint32_t maxDescriptorSets = 0;

    public:
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

        // // ----------- CORE REPOSITORIES
        CoreFrameBuffers coreFrameBuffers{context};
        CorePipelines corePipelines{context};
        CoreBuffers coreBuffers{context};
        CoreDescriptorSets coreDescriptorSets{context};
        // ----------- CORE REPOSITORIES

        // ----------- Services
        TextureService textureService{context};
        FrameBufferService framebufferService{context};
        PipelineService pipelineService{context};
        ShaderService shaderService{context};
        CommandService commandService{context};
        BufferService bufferService{context};
        DescriptorService descriptorService{context};
        // ----------- Services

        void dispose() const;

        void onInitialize() override;

        FrameData &getFrameData();

        [[nodiscard]] uint32_t getWindowWidth() const;

        [[nodiscard]] uint32_t getWindowHeight() const;

        void registerDescriptorBinding(VkDescriptorType type);

        void registerDescriptorSet();
    };
} // Metal

#endif
