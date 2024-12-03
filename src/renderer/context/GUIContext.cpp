#include "GUIContext.h"
#include "../VulkanUtils.h"

namespace Metal {
    GLFWContext &GUIContext::getWindowContext() {
        return windowContext;
    }

    VulkanContext &GUIContext::getVulkanContext() {
        return windowContext.getVulkanContext();
    }

    void GUIContext::shutdown() {
        VkResult err = vkDeviceWaitIdle(getVulkanContext().g_Device);
        Metal::VulkanUtils::check_vk_result(err);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImGui_ImplVulkanH_DestroyWindow(getVulkanContext().g_Instance, getVulkanContext().g_Device,
                                        &windowContext.getGUIWindow(), getVulkanContext().g_Allocator);
        windowContext.shutdown();
    }

    bool GUIContext::beginFrame() {
        if (!windowContext.beginFrame()) {
            // Start the Dear ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            return false;
        }
        return true;
    }

    void GUIContext::endFrame() {
        ImGui::Render();
        ImDrawData *main_draw_data = ImGui::GetDrawData();
        const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
        if (!main_is_minimized) {
            getVulkanContext().renderFrame(main_draw_data);
        }
    }

    void GUIContext::build(bool debugMode) {
        windowContext.build(debugMode);
        if (windowContext.isValidContext()) {
            setupContext();
        }
    }

    void GUIContext::setupContext() {

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(windowContext.getWindow(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = getVulkanContext().g_Instance;
        init_info.PhysicalDevice = getVulkanContext().g_PhysicalDevice;
        init_info.Device = getVulkanContext().g_Device;
        init_info.QueueFamily = getVulkanContext().g_QueueFamily;
        init_info.Queue = getVulkanContext().g_Queue;
        init_info.PipelineCache = getVulkanContext().g_PipelineCache;
        init_info.DescriptorPool = getVulkanContext().g_DescriptorPool;
        init_info.RenderPass = getVulkanContext().g_MainWindowData.RenderPass;
        init_info.Subpass = 0;
        init_info.MinImageCount = getVulkanContext().g_MinImageCount;
        init_info.ImageCount = getVulkanContext().g_MainWindowData.ImageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = getVulkanContext().g_Allocator;
        init_info.CheckVkResultFn = Metal::VulkanUtils::check_vk_result;
        ImGui_ImplVulkan_Init(&init_info);

        // Load Fonts

    }

    void GUIContext::start() {
        while (!glfwWindowShouldClose(windowContext.getWindow())) {
            if (beginFrame()) {
                continue;
            }
            root->render();
            endFrame();
        }
    }

    void GUIContext::setRoot(IPanel *root) {
        GUIContext::root = root;
    }
}