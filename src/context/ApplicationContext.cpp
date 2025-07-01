#include "ApplicationContext.h"

#include <imgui_impl_glfw.h>
#include <nfd.h>
#include <cereal/archives/binary.hpp>

#include "../common/FilesUtil.h"
#include "../renderer/util/VulkanUtils.h"

#include "../ui/util/FileDialogUtil.h"

namespace Metal
{
    ApplicationSingletons& ApplicationContext::getSingletons() const
    {
        return *applicationSingletons;
    }

    void ApplicationContext::updateRootPath(bool forceSelection)
    {
        std::string cachedPath;
        std::string cachePathFile = std::filesystem::current_path().string() + CACHED_PATH;
        FilesUtil::ReadFile(cachePathFile.c_str(), cachedPath);
        cachedPath.erase(std::ranges::remove(cachedPath, '\n').begin(), cachedPath.cend());
        if (cachedPath.empty() || forceSelection || !fs::exists(cachedPath))
        {
            rootDirectory = FileDialogUtil::SelectDirectory();
            rootDirectory.erase(std::ranges::remove(rootDirectory, '\n').begin(), rootDirectory.cend());
            if (rootDirectory.empty())
            {
                throw std::runtime_error("No directory selected.");
            }
            save();
            FilesUtil::WriteFile(cachePathFile.c_str(), rootDirectory.c_str());
        }
        else
        {
            rootDirectory = cachedPath;
        }
        PARSE_TEMPLATE(applicationSingletons->editorRepository.load,
                       rootDirectory + "/" + HASH_OF_CLASS_NAME(EditorRepository))
        PARSE_TEMPLATE(applicationSingletons->engineRepository.load,
                       rootDirectory + "/" + HASH_OF_CLASS_NAME(EngineRepository))
        PARSE_TEMPLATE(applicationSingletons->worldRepository.load,
                       rootDirectory + "/" + HASH_OF_CLASS_NAME(WorldRepository))

        FilesUtil::MkDir(getShadersDirectory());
        FilesUtil::MkDir(getAssetRefDirectory());
        FilesUtil::MkDir(getAssetDirectory());
    }

    unsigned int ApplicationContext::getFrameIndex() const
    {
        return applicationSingletons->vulkanContext.imguiVulkanWindow.FrameIndex;
    }

    void ApplicationContext::start()
    {
        applicationSingletons = std::make_unique<ApplicationSingletons>();
        NFD_Init();

        updateRootPath(false);

        applicationSingletons->glfwContext.onInitialize();
        if (!applicationSingletons->glfwContext.isValidContext())
        {
            throw std::runtime_error("Could not create window");
        }
        applicationSingletons->vulkanContext.onInitialize();
        applicationSingletons->guiContext.onInitialize();
        applicationSingletons->filesService.onInitialize();
        applicationSingletons->editorPanel.onInitialize();
        applicationSingletons->engineContext.onInitialize();

        GLFWwindow* window = applicationSingletons->glfwContext.getWindow();
        while (!glfwWindowShouldClose(window))
        {
            if (applicationSingletons->glfwContext.beginFrame())
            {
                GuiContext::BeginFrame();
                applicationSingletons->editorPanel.onSync();
                ImGui::Render();
                auto* drawData = ImGui::GetDrawData();
                const bool main_is_minimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);
                if (!main_is_minimized)
                {
                    applicationSingletons->vulkanContext.getCommandBuffers().clear();
                    auto& wd = applicationSingletons->vulkanContext.imguiVulkanWindow;
                    VkSemaphore imageAcquiredSemaphore = wd.FrameSemaphores[wd.SemaphoreIndex].ImageAcquiredSemaphore;
                    VkResult err = vkAcquireNextImageKHR(applicationSingletons->vulkanContext.device.device,
                                                         wd.Swapchain, UINT64_MAX,
                                                         imageAcquiredSemaphore, VK_NULL_HANDLE,
                                                         &wd.FrameIndex);
                    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
                    {
                        applicationSingletons->glfwContext.setSwapChainRebuild(true);
                        return;
                    }

                    VulkanUtils::CheckVKResult(err);
                    ImGui_ImplVulkanH_Frame* fd = &wd.Frames[getFrameIndex()];
                    VulkanUtils::CheckVKResult(vkWaitForFences(applicationSingletons->vulkanContext.device.device, 1,
                                                               &fd->Fence, VK_TRUE,
                                                               UINT64_MAX));
                    VulkanUtils::CheckVKResult(vkResetFences(applicationSingletons->vulkanContext.device.device, 1,
                                                             &fd->Fence));
                    VulkanUtils::CheckVKResult(vkResetCommandPool(applicationSingletons->vulkanContext.device.device,
                                                                  fd->CommandPool,
                                                                  VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
                    applicationSingletons->engineContext.onSync();
                    GuiContext::RecordImguiCommandBuffer(drawData, err, wd, fd);
                    applicationSingletons->vulkanContext.submitFrame(imageAcquiredSemaphore,
                                                                     wd.FrameSemaphores[wd.SemaphoreIndex].
                                                                     RenderCompleteSemaphore, fd);
                }
                if (!main_is_minimized)
                    applicationSingletons->glfwContext.presentFrame();
            }
        }
        NFD_Quit();
        applicationSingletons->guiContext.dispose();
        applicationSingletons->vulkanContext.dispose();
        applicationSingletons->glfwContext.dispose();
    }

    void ApplicationContext::save()
    {
        try
        {
            DUMP_TEMPLATE(rootDirectory + "/" + HASH_OF_CLASS_NAME(EditorRepository),
                          applicationSingletons->editorRepository)
            DUMP_TEMPLATE(rootDirectory + "/" + HASH_OF_CLASS_NAME(EngineRepository),
                          applicationSingletons->engineRepository)
            DUMP_TEMPLATE(rootDirectory + "/" + HASH_OF_CLASS_NAME(WorldRepository),
                          applicationSingletons->worldRepository)
            applicationSingletons->notificationService.pushMessage("Project saved", NotificationSeverities::SUCCESS);
        }
        catch (const std::exception& e)
        {
            applicationSingletons->notificationService.pushMessage("Could not save project",
                                                                   NotificationSeverities::ERROR);
        }
    }
}
