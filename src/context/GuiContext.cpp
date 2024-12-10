#include "GuiContext.h"

#include <imgui_impl_glfw.h>

#include "../common/interface/Icons.h"
#include "imgui_freetype.h"
#include "ApplicationContext.h"
#include "../common/util/VulkanUtils.h"


namespace Metal {
    static VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    bool GuiContext::beginFrame() {
        context.getVulkanContext().getFrameData().commandBuffers.clear();
        updateFrameData();
        if (!context.getGLFWContext().beginFrame()) {
            // Start the Dear ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            return false;
        }
        return true;
    }

    void GuiContext::updateFrameData() const {
        FrameData &frameData = context.getVulkanContext().getFrameData();

        auto &imguiVulkanWindow = context.getVulkanContext().imguiVulkanWindow;
        auto [CommandPool, CommandBuffer, Fence, Backbuffer, BackbufferView, Framebuffer] = imguiVulkanWindow.Frames[
                imguiVulkanWindow.FrameIndex];
        frameData.commandPool = CommandPool;
        frameData.commandBuffers.push_back(CommandBuffer);
        frameData.imguiCommandBuffer = CommandBuffer;
        frameData.fence = Fence;
        frameData.backbuffer = Backbuffer;
        frameData.backbufferView = BackbufferView;
        frameData.framebuffer = Framebuffer;
        frameData.imageAcquiredSemaphore = imguiVulkanWindow.FrameSemaphores[imguiVulkanWindow.SemaphoreIndex].
                ImageAcquiredSemaphore;
        frameData.renderCompleteSemaphore = imguiVulkanWindow.FrameSemaphores[imguiVulkanWindow.SemaphoreIndex].
                RenderCompleteSemaphore;
        frameData.frameIndex = imguiVulkanWindow.FrameIndex;
    }

    void GuiContext::recordCommandBuffer() const {
        FrameData &frameData = context.getVulkanContext().getFrameData();

        ImGui_ImplVulkanH_Window &imguiVkWindow = context.getVulkanContext().imguiVulkanWindow;

        VkCommandBufferBeginInfo bufferBeginInfo = {};
        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VulkanUtils::CheckVKResult(vkBeginCommandBuffer(frameData.imguiCommandBuffer, &bufferBeginInfo));

        VkRenderPassBeginInfo passBeginInfo = {};
        passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        passBeginInfo.renderPass = imguiVkWindow.RenderPass;
        passBeginInfo.framebuffer = frameData.framebuffer;
        passBeginInfo.renderArea.extent.width = imguiVkWindow.Width;
        passBeginInfo.renderArea.extent.height = imguiVkWindow.Height;
        passBeginInfo.clearValueCount = 1;
        passBeginInfo.pClearValues = &imguiVkWindow.ClearValue;
        vkCmdBeginRenderPass(frameData.imguiCommandBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Record dear imgui primitives into command vkBuffer
        ImGui_ImplVulkan_RenderDrawData(drawData, frameData.imguiCommandBuffer);

        // Submit command vkBuffer
        vkCmdEndRenderPass(frameData.imguiCommandBuffer);
        VulkanUtils::CheckVKResult(vkEndCommandBuffer(frameData.imguiCommandBuffer));
    }

    void GuiContext::frameRender() const {
        FrameData &frameData = context.getVulkanContext().getFrameData();

        ImGui_ImplVulkanH_Window &imguiVkWindow = context.getVulkanContext().imguiVulkanWindow;
        const VkDevice &device = context.getVulkanContext().device.device;

        const VkResult err = vkAcquireNextImageKHR(device, imguiVkWindow.Swapchain, UINT64_MAX,
                                                   frameData.imageAcquiredSemaphore,
                                                   VK_NULL_HANDLE,
                                                   &frameData.frameIndex);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            context.getGLFWContext().setSwapChainRebuild(true);
            return;
        }
        VulkanUtils::CheckVKResult(err);
        // wait indefinitely instead of periodically checking
        VulkanUtils::CheckVKResult(vkWaitForFences(device, 1, &frameData.fence, VK_TRUE,
                                                   UINT64_MAX));
        VulkanUtils::CheckVKResult(vkResetFences(device, 1, &frameData.fence));
        VulkanUtils::CheckVKResult(vkResetCommandPool(device, frameData.commandPool, 0));

        recordCommandBuffer();

        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &frameData.imageAcquiredSemaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = frameData.commandBuffers.size();
        info.pCommandBuffers = frameData.commandBuffers.data();
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &frameData.renderCompleteSemaphore;
        VulkanUtils::CheckVKResult(vkQueueSubmit(context.getVulkanContext().graphicsQueue, 1, &info,
                                                 frameData.fence));
    }

    void GuiContext::endFrame() {
        ImGui::Render();
        drawData = ImGui::GetDrawData();
        if (!(drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f)) {
            frameRender();
            VulkanUtils::FramePresent(&context.getVulkanContext().imguiVulkanWindow,
                                      context.getVulkanContext().graphicsQueue,
                                      context.getGLFWContext().isSwapChainRebuild());
        }
    }

    void GuiContext::onInitialize() {
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
        init_info.Instance = context.getVulkanContext().instance.instance;
        init_info.PhysicalDevice = context.getVulkanContext().physDevice.physical_device;
        init_info.Device = context.getVulkanContext().device.device;
        init_info.QueueFamily = context.getVulkanContext().queueFamily;
        init_info.Queue = context.getVulkanContext().graphicsQueue;
        init_info.PipelineCache = context.getVulkanContext().pipelineCache;
        init_info.DescriptorPool = context.getVulkanContext().descriptorPool;
        init_info.RenderPass = context.getVulkanContext().imguiVulkanWindow.RenderPass;
        init_info.Subpass = 0;
        init_info.MinImageCount = IMAGE_COUNT;
        init_info.ImageCount = context.getVulkanContext().imguiVulkanWindow.ImageCount;
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

        io.Fonts->Build();
        delete fontConfig;
    }

    void GuiContext::dispose() const {
        const VkResult err = vkDeviceWaitIdle(context.getVulkanContext().device.device);
        VulkanUtils::CheckVKResult(err);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImGui_ImplVulkanH_DestroyWindow(context.getVulkanContext().instance, context.getVulkanContext().device.device,
                                        &context.getGLFWContext().getGUIWindow(),
                                        nullptr);
        context.getGLFWContext().dispose();
    }

    GuiContext::GuiContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {}
}
