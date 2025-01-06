#include "GlobalIlluminationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../util/ImageUtils.h"

namespace Metal {
    bool GlobalIlluminationPass::shouldRun() {
        return context.engineRepository.giEnabled;
    }

    PipelineInstance *GlobalIlluminationPass::getPipeline() {
        return context.corePipelines.giComputePipeline;
    }

    void GlobalIlluminationPass::clearBuffer() {
        if (isFirstRun || context.engineContext.shouldClearGIBuffer()) {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = context.coreTextures.globalIllumination->vkImage;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(
                vkCommandBuffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);


            VkImageLayout layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            VkClearColorValue color = {.float32 = {0, 0, 0, 1}};
            VkImageSubresourceRange imageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
            vkCmdClearColorImage(vkCommandBuffer, context.coreTextures.globalIllumination->vkImage, layout, &color, 1,
                                 &imageSubresourceRange);
            context.engineContext.resetFrameCount();
            isFirstRun = false;
        }
    }

    void GlobalIlluminationPass::onSync() {
        clearBuffer();

        // Convert image layout to GENERAL before writing into it in compute shader.
        VkImageMemoryBarrier read2Gen = ImageUtils::ReadOnlyToGeneralBarrier(
            context.coreTextures.globalIllumination->vkImage);

        vkCmdPipelineBarrier(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &read2Gen);

        recordImageDispatch(context.coreFrameBuffers.auxFBO, 16, 16);

        // // Convert image layout to READ_ONLY_OPTIMAL before reading from it in fragment shader.
        VkImageMemoryBarrier write2ReadBarrier = ImageUtils::writeToReadOnlyBarrier(
            context.coreTextures.globalIllumination->vkImage);

        vkCmdPipelineBarrier(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &write2ReadBarrier);
    }
} // Metal
