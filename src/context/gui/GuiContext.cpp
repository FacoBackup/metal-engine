#include "GuiContext.h"

#include <imgui_impl_glfw.h>
#include "imgui_freetype.h"
#include "../ApplicationContext.h"
#include "../../util/VulkanUtils.h"
#include "../../repository/descriptors/CoreDescriptorSets.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferAttachment.h"
#include "../../service/texture/TextureInstance.h"

namespace Metal {
    void GuiContext::endFrame() {

    }

    void GuiContext::renderImage(TextureInstance *texture, const float sizeX, const float sizeY) const {
        context.descriptorService.setImageDescriptor(texture);
        ImGui::Image(reinterpret_cast<ImTextureID>(texture->imageDescriptor->vkDescriptorSet), ImVec2{sizeX, sizeY});
    }

    void GuiContext::BeginFrame() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GuiContext::RecordImguiCommandBuffer(ImDrawData *drawData, VkResult &err, ImGui_ImplVulkanH_Window &wd,
                                              ImGui_ImplVulkanH_Frame *fd) {
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
        VulkanUtils::CheckVKResult(err); {
            VkRenderPassBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass = wd.RenderPass;
            info.framebuffer = fd->Framebuffer;
            info.renderArea.extent.width = wd.Width;
            info.renderArea.extent.height = wd.Height;
            info.clearValueCount = 1;
            info.pClearValues = &wd.ClearValue;
            vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(drawData, fd->CommandBuffer);

        // Submit command buffer
        vkCmdEndRenderPass(fd->CommandBuffer);
    }

    void GuiContext::onInitialize() {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

        io.ConfigViewportsNoTaskBarIcon = true;
        io.ConfigDockingAlwaysTabBar = true;
        io.ConfigWindowsResizeFromEdges = true;

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(context.glfwContext.getWindow(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = context.vulkanContext.instance.instance;
        init_info.PhysicalDevice = context.vulkanContext.physDevice.physical_device;
        init_info.Device = context.vulkanContext.device.device;
        init_info.QueueFamily = context.vulkanContext.queueFamily;
        init_info.Queue = context.vulkanContext.graphicsQueue;
        init_info.PipelineCache = context.vulkanContext.pipelineCache;
        init_info.DescriptorPool = context.vulkanContext.descriptorPool;
        init_info.RenderPass = context.vulkanContext.imguiVulkanWindow.RenderPass;
        init_info.Subpass = 0;
        init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
        init_info.ImageCount = context.vulkanContext.imguiVulkanWindow.ImageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = VulkanUtils::CheckVKResult;
        ImGui_ImplVulkan_Init(&init_info);

        applySpacing();
        applyFonts();
    }

    void GuiContext::applySpacing() {
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

    void GuiContext::applyFonts() {
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();

        auto *fontConfig = new ImFontConfig();
        fontConfig->PixelSnapH = true;

        io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Regular.ttf", 14, fontConfig,
                                     io.Fonts->GetGlyphRangesDefault());
        fontConfig->MergeMode = true;
        fontConfig->GlyphOffset = ImVec2(-2, 4);
        constexpr auto MIN = static_cast<ImWchar16>(0xF1000);
        constexpr auto MAX = static_cast<ImWchar16>(0xFF000);
        constexpr ImWchar RANGES[] = {MIN, MAX, 0};
        io.Fonts->AddFontFromFileTTF("resources/fonts/MaterialIcons.ttf", 18, fontConfig, RANGES);
        largeIconsFont = io.Fonts->AddFontFromFileTTF("resources/fonts/MaterialIcons.ttf", LARGE_FONT_SIZE, nullptr,
                                                      RANGES);

        io.Fonts->Build();
        delete fontConfig;
    }

    void GuiContext::dispose() const {
        const VkResult err = vkDeviceWaitIdle(context.vulkanContext.device.device);
        VulkanUtils::CheckVKResult(err);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImGui_ImplVulkanH_DestroyWindow(context.vulkanContext.instance, context.vulkanContext.device.device,
                                        &context.glfwContext.getGUIWindow(),
                                        nullptr);
    }

    GuiContext::GuiContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }
}
