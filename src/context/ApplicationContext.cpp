#include "ApplicationContext.h"

#include <imgui_impl_glfw.h>
#include <nfd.h>
#include <cereal/archives/binary.hpp>

#include "../util/FilesUtil.h"
#include "../util/VulkanUtils.h"

#include "../util/FileDialogUtil.h"

namespace Metal {
    void ApplicationContext::updateRootPath(bool forceSelection) {
        std::string cachedPath;
        std::string cachePathFile = std::filesystem::current_path().string() + CACHED_PATH;
        FilesUtil::ReadFile(cachePathFile.c_str(), cachedPath);
        cachedPath.erase(std::ranges::remove(cachedPath, '\n').begin(), cachedPath.cend());
        if (cachedPath.empty() || forceSelection || !fs::exists(cachedPath)) {
            rootDirectory = FileDialogUtil::SelectDirectory();
            rootDirectory.erase(std::ranges::remove(rootDirectory, '\n').begin(), rootDirectory.cend());
            if (rootDirectory.empty()) {
                throw std::runtime_error("No directory selected.");
            }
            save();
            FilesUtil::WriteFile(cachePathFile.c_str(), rootDirectory.c_str());
        } else {
            rootDirectory = cachedPath;
        }
        PARSE_TEMPLATE(editorRepository.load, rootDirectory + "/" + HASH_OF_CLASS_NAME(EditorRepository))
        PARSE_TEMPLATE(engineRepository.load, rootDirectory + "/" + HASH_OF_CLASS_NAME(EngineRepository))
        PARSE_TEMPLATE(worldGridRepository.load, rootDirectory + "/" + HASH_OF_CLASS_NAME(WorldGridRepository))
        PARSE_TEMPLATE(worldRepository.load, rootDirectory + "/" + HASH_OF_CLASS_NAME(WorldRepository))


        FilesUtil::MkDir(getShadersDirectory());
        FilesUtil::MkDir(getAssetRefDirectory());
        FilesUtil::MkDir(getAssetDirectory());
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
        filesService.onInitialize();
        editorPanel.onInitialize();
        engineContext.onInitialize();

        GLFWwindow *window = glfwContext.getWindow();
        while (!glfwWindowShouldClose(window)) {
            if (glfwContext.beginFrame()) {
                GuiContext::BeginFrame();
                editorPanel.onSync();
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
                        glfwContext.setSwapChainRebuild(true);
                        return;
                    }

                    VulkanUtils::CheckVKResult(err);
                    ImGui_ImplVulkanH_Frame *fd = &wd.Frames[getFrameIndex()];
                    VulkanUtils::CheckVKResult(vkWaitForFences(vulkanContext.device.device, 1, &fd->Fence, VK_TRUE,
                                                               UINT64_MAX));
                    VulkanUtils::CheckVKResult(vkResetFences(vulkanContext.device.device, 1, &fd->Fence));
                    VulkanUtils::CheckVKResult(vkResetCommandPool(vulkanContext.device.device, fd->CommandPool,
                                                                  VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
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

    void ApplicationContext::save() {
        try {
            DUMP_TEMPLATE(rootDirectory + "/" + HASH_OF_CLASS_NAME(EditorRepository), editorRepository)
            DUMP_TEMPLATE(rootDirectory + "/" + HASH_OF_CLASS_NAME(EngineRepository), engineRepository)
            DUMP_TEMPLATE(rootDirectory + "/" + HASH_OF_CLASS_NAME(WorldGridRepository), worldGridRepository)
            DUMP_TEMPLATE(rootDirectory + "/" + HASH_OF_CLASS_NAME(WorldRepository), worldRepository)
            notificationService.pushMessage("Project saved", NotificationSeverities::SUCCESS);
        }catch (const std::exception &e) {
            notificationService.pushMessage("Could not save project", NotificationSeverities::ERROR);
        }
    }
}
