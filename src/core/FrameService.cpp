#include "FrameService.h"

#include <nfd.h>
#include <SDL3/SDL.h>
#include <imgui_impl_sdl3.h>

#include "ImGuiService.h"
#include "VulkanContext.h"
#include "WindowService.h"
#include "../editor/abstract/AbstractPanel.h"
#include "../engine/EngineContext.h"
#include "../common/VulkanUtils.h"

namespace Metal {
    unsigned int FrameService::getFrameIndex() const {
        return vulkanContext->imguiVulkanWindow.FrameIndex;
    }

    bool FrameService::isValidContext() const {
        return windowService->isValidContext();
    }

    void FrameService::setPanel(AbstractPanel *panel) {
        this->panel = panel;
    }

    void FrameService::start() const {
        SDL_Window *window = windowService->getWindow();
        bool done = false;
        while (!done) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                ImGui_ImplSDL3_ProcessEvent(&event);
                if (event.type == SDL_EVENT_QUIT)
                    done = true;
                if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                    done = true;
            }
            if (imguiService->beginFrame()) {
                panel->onSync();
                ImGui::Render();
                auto *drawData = ImGui::GetDrawData();
                const bool main_is_minimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);
                if (!main_is_minimized) {
                    vulkanContext->getCommandBuffers().clear();
                    auto &wd = vulkanContext->imguiVulkanWindow;
                    VkSemaphore imageAcquiredSemaphore = wd.FrameSemaphores[wd.SemaphoreIndex].ImageAcquiredSemaphore;
                    VkResult err = vkAcquireNextImageKHR(vulkanContext->device.device, wd.Swapchain, UINT64_MAX,
                                                         imageAcquiredSemaphore, VK_NULL_HANDLE,
                                                         &wd.FrameIndex);
                    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
                        imguiService->setSwapChainRebuild(true);
                        return;
                    }

                    VulkanUtils::CheckVKResult(err);
                    ImGui_ImplVulkanH_Frame *fd = &wd.Frames[getFrameIndex()];
                    VulkanUtils::CheckVKResult(vkWaitForFences(vulkanContext->device.device, 1, &fd->Fence, VK_TRUE,
                                                               UINT64_MAX));
                    VulkanUtils::CheckVKResult(vkResetFences(vulkanContext->device.device, 1, &fd->Fence));
                    VulkanUtils::CheckVKResult(vkResetCommandPool(vulkanContext->device.device, fd->CommandPool,
                                                                  VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
                    engineContext->onSync();
                    ImGuiService::RecordImguiCommandBuffer(drawData, err, wd, fd);
                    vulkanContext->submitFrame(imageAcquiredSemaphore, wd.FrameSemaphores[wd.SemaphoreIndex].
                                               RenderCompleteSemaphore, fd);
                }
                if (!main_is_minimized)
                    imguiService->presentFrame();
            }
        }
    }
} // Metal
