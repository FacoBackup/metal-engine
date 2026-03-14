#ifndef METAL_ENGINE_VULKANCONTEXT_H
#define METAL_ENGINE_VULKANCONTEXT_H

#include "vk_mem_alloc.h"

#include "imgui_impl_vulkan.h"
#include <GLFW/glfw3.h>
#include "VkBootstrap.h"
#include "../../common/IInit.h"
#include "../../common/IService.h"
#include "../../common/IDisposable.h"

#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

namespace Metal {
    class GLFWContext;
    struct EngineRepository;
    class MeshService;
    class TextureService;
    class FrameBufferService;
    class PipelineService;
    class RayTracingService;

    class VulkanContext final : public IService, public IDisposable, public IInit {
        GLFWContext *glfwContext = nullptr;
        EngineRepository *engineRepository = nullptr;
        MeshService *meshService = nullptr;
        TextureService *textureService = nullptr;
        FrameBufferService *framebufferService = nullptr;
        PipelineService *pipelineService = nullptr;
        RayTracingService *rayTracingService = nullptr;

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

        unsigned int w{}, h{};
        GLFWwindow *window = nullptr;
        bool debugMode = false;
        std::vector<VkCommandBuffer> commandBuffers{};

    public:
        void pushCommandBuffer(VkCommandBuffer commandBuffer) {
            commandBuffers.push_back(commandBuffer);
        }

        std::vector<VkCommandBuffer> &getCommandBuffers() { return commandBuffers; }

        std::vector<Dependency> getDependencies() override {
            return {
                {"GLFWContext", glfwContext},
                {"EngineRepository", engineRepository},
                {"MeshService", meshService},
                {"TextureService", textureService},
                {"FrameBufferService", framebufferService},
                {"PipelineService", pipelineService},
                {"RayTracingService", rayTracingService}
            };
        }

        VkPhysicalDeviceProperties physicalDeviceProperties{};
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};
        vkb::Instance instance{};
        ImGui_ImplVulkanH_Window imguiVulkanWindow;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        unsigned int queueFamily{};
        vkb::PhysicalDevice physDevice;
        vkb::Device device;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        VmaAllocator allocator = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties{};
        VkSampler vkImageSampler = VK_NULL_HANDLE;
        VkSampler vkTextureSampler = VK_NULL_HANDLE;

        PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR = nullptr;
        PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR = nullptr;
        PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR = nullptr;
        PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR = nullptr;
        PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR = nullptr;
        PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR = nullptr;
        PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR = nullptr;
        PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR = nullptr;
        PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR = nullptr;

        void dispose() override;

        void onInitialize() override;

        [[nodiscard]] unsigned int getWindowHeight() const {
            return h;
        }

        [[nodiscard]] unsigned int getWindowWidth() const {
            return w;
        }

        [[nodiscard]] VkCommandBuffer beginSingleTimeCommands() const;

        void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

        void submitFrame(VkSemaphore image_acquired_semaphore, VkSemaphore render_complete_semaphore,
                         ImGui_ImplVulkanH_Frame *fd);
    };
} // Metal

#endif
