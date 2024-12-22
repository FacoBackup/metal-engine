#include "ApplicationContext.h"

#include <imgui_impl_glfw.h>
#include <nfd.h>
#include "../common/util/files/FilesUtil.h"
#include "../common/util/VulkanUtils.h"

#include "../common/util/files/FileDialogUtil.h"

namespace Metal {
    void ApplicationContext::updateRootPath(bool forceSelection) {
        std::string cachedPath;
        std::string cachePathFile = std::filesystem::current_path().string() + "/metal-engine-cached.txt";
        FilesUtil::ReadFile(cachePathFile.c_str(), cachedPath);

        if (cachedPath.empty() || forceSelection) {
            rootDirectory = FileDialogUtil::SelectDirectory();
            if (rootDirectory.empty()) {
                throw std::runtime_error("No directory selected.");
            }
            FilesUtil::WriteFile(cachePathFile.c_str(), rootDirectory.c_str());
        } else {
            rootDirectory = cachedPath;
        }
        rootDirectory.erase(std::ranges::remove(rootDirectory, '\n').begin(), rootDirectory.cend());
        FilesUtil::ReadFile((rootDirectory + "/" + PROJECT_METADATA_FILE).c_str(), projectName);
        if (projectName.empty()) {
            updateProjectName("New project");
        }
        FilesUtil::MkDir(getShadersDirectory());
        FilesUtil::MkDir(getAssetRefDirectory());
        FilesUtil::MkDir(getAssetDirectory());
    }

    void ApplicationContext::updateProjectName(const std::string &projectName) {
        FilesUtil::WriteFile((rootDirectory + "/" + PROJECT_METADATA_FILE).c_str(), projectName.c_str());
        this->projectName = projectName;
    }

    uint32_t ApplicationContext::getFrameIndex() const {
        return vulkanContext.imguiVulkanWindow.FrameIndex;
    }

    void ApplicationContext::start() {
        NFD_Init();

        updateRootPath(false);

        glfwContext.onInitialize();
        if (!glfwContext.isValidContext()) {
            throw std::runtime_error("Could not create window");
        }
        vulkanContext.onInitialize();
        guiContext.onInitialize();
        editorContext.onInitialize();
        rootPanel.onInitialize();

        GLFWwindow *window = glfwContext.getWindow();
        while (!glfwWindowShouldClose(window)) {
            if (glfwContext.beginFrame()) {
                GuiContext::BeginFrame();
                rootPanel.onSync();
                ImGui::Render();
                auto *drawData = ImGui::GetDrawData();
                const bool main_is_minimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);
                if (!main_is_minimized) {
                    vulkanContext.getCommandBuffers().clear();
                    auto &wd = vulkanContext.imguiVulkanWindow;
                    VkSemaphore imageAcquiredSemaphore = wd.FrameSemaphores[wd.SemaphoreIndex].ImageAcquiredSemaphore;
                    VkResult err = vkAcquireNextImageKHR(vulkanContext.device.device, wd.Swapchain, UINT64_MAX,
                                                         imageAcquiredSemaphore, VK_NULL_HANDLE,
                                                         &wd.FrameIndex);
                    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
                        getGLFWContext().setSwapChainRebuild(true);
                        return;
                    }

                    VulkanUtils::CheckVKResult(err);
                    ImGui_ImplVulkanH_Frame *fd = &wd.Frames[getFrameIndex()];
                    VulkanUtils::CheckVKResult(vkWaitForFences(vulkanContext.device.device, 1, &fd->Fence, VK_TRUE,
                                                               UINT64_MAX));
                    VulkanUtils::CheckVKResult(vkResetFences(vulkanContext.device.device, 1, &fd->Fence));
                    VulkanUtils::CheckVKResult(vkResetCommandPool(vulkanContext.device.device, fd->CommandPool, 0));
                    engineContext.onSync();
                    GuiContext::RecordImguiCommandBuffer(drawData, err, wd, fd);
                    vulkanContext.submitFrame(imageAcquiredSemaphore, wd.FrameSemaphores[wd.SemaphoreIndex].
                            RenderCompleteSemaphore, fd);
                }
                if (!main_is_minimized)
                    glfwContext.presentFrame();
            }
        }
        NFD_Quit();
        guiContext.dispose();
        vulkanContext.dispose();
        glfwContext.dispose();
    }

    ApplicationContext::ApplicationContext(IPanel &root_panel, bool debugMode) : vulkanContext(*this, debugMode),
        rootPanel(root_panel) {
    }
}
