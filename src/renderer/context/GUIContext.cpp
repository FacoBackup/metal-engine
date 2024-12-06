#include "GUIContext.h"

#include <imgui_impl_glfw.h>

#include "../VulkanUtils.h"
#include "../../common/Icons.h"
#include "imgui_freetype.h"
#include "../../common/runtime/ApplicationContext.h"
#define IMGUI_ENABLE_FREETYPE

namespace Metal {

    void GUIContext::shutdown() const {
        const VkResult err = vkDeviceWaitIdle(context.getVulkanContext().g_Device);
        VulkanUtils::check_vk_result(err);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImGui_ImplVulkanH_DestroyWindow(context.getVulkanContext().g_Instance, context.getVulkanContext().g_Device,
                                        &context.getGLFWContext().getGUIWindow(), context.getVulkanContext().g_Allocator);
        context.getGLFWContext().shutdown();
    }

    bool GUIContext::beginFrame() const {
        if (!context.getGLFWContext().beginFrame()) {
            // Start the Dear ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            return false;
        }
        return true;
    }

    void GUIContext::endFrame() const {
        ImGui::Render();
        ImDrawData *main_draw_data = ImGui::GetDrawData();
        const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
        if (!main_is_minimized) {
            context.getVulkanContext().renderFrame(main_draw_data);
        }
    }

    void GUIContext::build(const bool debugMode) const {
        context.getGLFWContext().build(debugMode);
        if (context.getGLFWContext().isValidContext()) {
            setupContext();
        }
    }

    void GUIContext::setupContext() const {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

        io.ConfigViewportsNoTaskBarIcon = true;
        io.ConfigDockingAlwaysTabBar = true;
        io.ConfigWindowsResizeFromEdges = true;

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(context.getGLFWContext().getWindow(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = context.getVulkanContext().g_Instance;
        init_info.PhysicalDevice = context.getVulkanContext().g_PhysicalDevice;
        init_info.Device = context.getVulkanContext().g_Device;
        init_info.QueueFamily = context.getVulkanContext().g_QueueFamily;
        init_info.Queue = context.getVulkanContext().g_Queue;
        init_info.PipelineCache = context.getVulkanContext().g_PipelineCache;
        init_info.DescriptorPool = context.getVulkanContext().g_DescriptorPool;
        init_info.RenderPass = context.getVulkanContext().g_MainWindowData.RenderPass;
        init_info.Subpass = 0;
        init_info.MinImageCount = context.getVulkanContext().g_MinImageCount;
        init_info.ImageCount = context.getVulkanContext().g_MainWindowData.ImageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = context.getVulkanContext().g_Allocator;
        init_info.CheckVkResultFn = VulkanUtils::check_vk_result;
        ImGui_ImplVulkan_Init(&init_info);

        applySpacing();
        applyFonts();
    }

    void GUIContext::applySpacing() {
        ImGuiStyle &style = ImGui::GetStyle();
        constexpr float borderRadius = 3.f;
        constexpr float borderWidth = 1;

        style.WindowMinSize = ImVec2(25.f, 25.f);
        style.WindowPadding = ImVec2(8.f, 8.f);
        style.FramePadding = ImVec2(5.f, 5.f);
        style.CellPadding = ImVec2(6.f, 5.f);
        style.ItemSpacing = ImVec2(6.f, 5.f);
        style.ItemInnerSpacing = ImVec2(6.f, 6.f);
        style.TouchExtraPadding = ImVec2(0.f, 0.f);
        style.IndentSpacing = 25.f;
        style.ScrollbarSize = 13.f;
        style.GrabMinSize = 10.f;
        style.WindowBorderSize = borderWidth;
        style.ChildBorderSize = borderWidth;
        style.PopupBorderSize = borderWidth;
        style.FrameBorderSize = borderWidth;
        style.TabBorderSize = borderWidth;
        style.WindowRounding = 0;
        style.ChildRounding = borderRadius;
        style.FrameRounding = borderRadius;
        style.PopupRounding = borderRadius;
        style.ScrollbarRounding = 9.f;
        style.GrabRounding = borderRadius;
        style.LogSliderDeadzone = 4.f;
        style.TabRounding = borderRadius;
        style.Alpha = 1;
    }

    void GUIContext::applyFonts() {
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();

        auto *fontConfig = new ImFontConfig();
        fontConfig->PixelSnapH = true;

        io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 14, fontConfig,
                                     io.Fonts->GetGlyphRangesDefault());
        fontConfig->MergeMode = true;
        fontConfig->GlyphOffset = ImVec2(-2, 4);
        constexpr auto MIN = static_cast<ImWchar16>(0xF1000);
        constexpr auto MAX = static_cast<ImWchar16>(0xFF000);
        constexpr ImWchar RANGES[] = {MIN, MAX, 0};
        io.Fonts->AddFontFromFileTTF("fonts/MaterialIcons.ttf", 18, fontConfig, RANGES);

        io.Fonts->Build();
        delete fontConfig;
    }
}
