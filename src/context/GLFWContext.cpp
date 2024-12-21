#include <imgui_impl_glfw.h>
#include "GLFWContext.h"
#include "ApplicationContext.h"
#include "VulkanContext.h"
#include "../common/util/VulkanUtils.h"

namespace Metal {
    GLFWwindow *GLFWContext::getWindow() const {
        return window;
    }

    bool GLFWContext::isValidContext() const {
        return validContext;
    }

    bool GLFWContext::beginFrame() {
        glfwPollEvents();

        // Resize swap chain?
        int fb_width, fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        if (fb_width > 0 && fb_height > 0 &&
            (swapChainRebuild || context.vulkanContext.imguiVulkanWindow.Width !=
             fb_width ||
             context.vulkanContext.imguiVulkanWindow.Height != fb_height)) {
            ImGui_ImplVulkan_SetMinImageCount(IMAGE_COUNT);
            ImGui_ImplVulkanH_CreateOrResizeWindow(context.vulkanContext.instance.instance,
                                                   context.vulkanContext.physDevice.physical_device,
                                                   context.vulkanContext.device.device,
                                                   &context.vulkanContext.imguiVulkanWindow,
                                                   context.vulkanContext.queueFamily,
                                                   nullptr, fb_width,
                                                   fb_height,
                                                   IMAGE_COUNT);
            context.vulkanContext.imguiVulkanWindow.FrameIndex = 0;
            swapChainRebuild = false;
        }
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            return false;
        }
        return true;
    }

    void GLFWContext::dispose() const {
        context.vulkanContext.dispose();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    ImGui_ImplVulkanH_Window &GLFWContext::getGUIWindow() const {
        return context.vulkanContext.imguiVulkanWindow;
    }

    void GLFWContext::setSwapChainRebuild(const bool val) {
        swapChainRebuild = val;
    }

    void GLFWContext::presentFrame() {
        if (swapChainRebuild) {
            return;
        }
        auto &wd = context.vulkanContext.imguiVulkanWindow;
        VkSemaphore semaphore = wd.FrameSemaphores[wd.SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &semaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &wd.Swapchain;
        info.pImageIndices = &wd.FrameIndex;
        VkResult err = vkQueuePresentKHR(context.vulkanContext.graphicsQueue, &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            swapChainRebuild = true;
            return;
        }
        VulkanUtils::CheckVKResult(err);
        wd.SemaphoreIndex =
                (wd.SemaphoreIndex + 1) % wd.SemaphoreCount; // Now we can use the next set of semaphores
    }

    void GLFWContext::renderFrame(ImDrawData *drawData) const {
        VkResult err;
        auto &wd = context.vulkanContext.imguiVulkanWindow;

        VkSemaphore image_acquired_semaphore = wd.FrameSemaphores[wd.SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore render_complete_semaphore = wd.FrameSemaphores[wd.SemaphoreIndex].RenderCompleteSemaphore;
        err = vkAcquireNextImageKHR(context.vulkanContext.device.device, wd.Swapchain, UINT64_MAX,
                                    image_acquired_semaphore, VK_NULL_HANDLE,
                                    &wd.FrameIndex);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            context.getGLFWContext().setSwapChainRebuild(true);
            return;
        }
        VulkanUtils::CheckVKResult(err);

        ImGui_ImplVulkanH_Frame *fd = &wd.Frames[wd.FrameIndex]; {
            err = vkWaitForFences(context.vulkanContext.device.device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);
            // wait indefinitely instead of periodically checking
            VulkanUtils::CheckVKResult(err);

            err = vkResetFences(context.vulkanContext.device.device, 1, &fd->Fence);
            VulkanUtils::CheckVKResult(err);
        } {
            err = vkResetCommandPool(context.vulkanContext.device.device, fd->CommandPool, 0);
            VulkanUtils::CheckVKResult(err);
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
            VulkanUtils::CheckVKResult(err);
        } {
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
        vkCmdEndRenderPass(fd->CommandBuffer); {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo info = {};
            context.vulkanContext.getFrameData().commandBuffers.push_back(fd->CommandBuffer);
            info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount = 1;
            info.pWaitSemaphores = &image_acquired_semaphore;
            info.pWaitDstStageMask = &wait_stage;
            info.commandBufferCount = context.vulkanContext.getFrameData().commandBuffers.size();
            info.pCommandBuffers = context.vulkanContext.getFrameData().commandBuffers.data();
            info.signalSemaphoreCount = 1;
            info.pSignalSemaphores = &render_complete_semaphore;

            err = vkEndCommandBuffer(fd->CommandBuffer);
            VulkanUtils::CheckVKResult(err);
            err = vkQueueSubmit(context.vulkanContext.graphicsQueue, 1, &info, fd->Fence);
            VulkanUtils::CheckVKResult(err);
        }
    }

    void GLFWContext::onInitialize() {
        glfwSetErrorCallback(VulkanUtils::GLFWErrorCallback);
        if (!glfwInit()) {
            validContext = false;
        }

        if (validContext) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            window = glfwCreateWindow(1280, 720, ENGINE_NAME, nullptr, nullptr);

            if (!glfwVulkanSupported()) {
                validContext = false;
            } else {
                validContext = true;
                uint32_t extensions_count = 0;
                const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
                for (uint32_t i = 0; i < extensions_count; i++)
                    instance_extensions.push_back(glfw_extensions[i]);
            }
        }
    }

    const ImVector<const char *> &GLFWContext::getInstanceExtensions() const {
        return instance_extensions;
    }
}
