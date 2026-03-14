#include "FrameService.h"

#include <nfd.h>
#include <GLFW/glfw3.h>

#include "gui/GuiContext.h"
#include "vulkan/VulkanContext.h"
#include "glfw/GLFWContext.h"
#include "../editor/abstract/AbstractPanel.h"
#include "../engine/EngineContext.h"
#include "vulkan/VulkanUtils.h"

namespace Metal {
    unsigned int FrameService::getFrameIndex() const {
        return vulkanContext->imguiVulkanWindow.FrameIndex;
    }

    void FrameService::dispose() {
        NFD_Quit();
    }

    bool FrameService::isValidContext() const {
        return glfwContext->isValidContext();
    }

    void FrameService::setPanel(AbstractPanel *panel) {
        this->panel = panel;
    }

    void FrameService::onInitialize() {
        NFD_Init();

        GLFWwindow *window = glfwContext->getWindow();
        while (!glfwWindowShouldClose(window)) {
            if (glfwContext->beginFrame()) {
                GuiContext::BeginFrame();
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
                        glfwContext->setSwapChainRebuild(true);
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
                    GuiContext::RecordImguiCommandBuffer(drawData, err, wd, fd);
                    vulkanContext->submitFrame(imageAcquiredSemaphore, wd.FrameSemaphores[wd.SemaphoreIndex].
                                              RenderCompleteSemaphore, fd);
                }
                if (!main_is_minimized)
                    glfwContext->presentFrame();
            }
        }
    }
} // Metal
