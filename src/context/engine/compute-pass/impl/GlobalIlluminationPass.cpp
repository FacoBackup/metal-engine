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

    void GlobalIlluminationPass::onSync() {
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

        // Bind compute pipeline and dispatch compute command.
        recordImageDispatch(context.coreTextures.globalIllumination, 8, 8);

        // Transfer target and accumulation images to transfer layout and copy one into another.
        VkImageMemoryBarrier gen2TranSrc = ImageUtils::generalToTransferSrcBarrier(
            context.coreTextures.globalIllumination->vkImage);

        vkCmdPipelineBarrier(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &gen2TranSrc);

        VkImageMemoryBarrier gen2TranDst = ImageUtils::generalToTransferDstBarrier(
            context.coreTextures.giAccumulation->vkImage);

        vkCmdPipelineBarrier(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &gen2TranDst);

        VkImageCopy region = ImageUtils::imageCopyRegion(context.coreTextures.globalIllumination->width,
                                                                context.coreTextures.globalIllumination->height);
        vkCmdCopyImage(
            vkCommandBuffer,
            context.coreTextures.globalIllumination->vkImage,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            context.coreTextures.giAccumulation->vkImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);

        VkImageMemoryBarrier tranDst2Gen = ImageUtils::transferDstToGeneralBarrier(
            context.coreTextures.giAccumulation->vkImage);

        vkCmdPipelineBarrier(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &tranDst2Gen);

        // Convert image layout to READ_ONLY_OPTIMAL before reading from it in fragment shader.
        VkImageMemoryBarrier tranSrc2ReadOnly = ImageUtils::transferSrcToReadOnlyBarrier(
            context.coreTextures.globalIllumination->vkImage);

        vkCmdPipelineBarrier(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &tranSrc2ReadOnly);
    }
} // Metal
