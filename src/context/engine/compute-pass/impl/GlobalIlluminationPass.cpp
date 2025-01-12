#include "GlobalIlluminationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/texture/TextureInstance.h"
#include "../../../../util/ImageUtils.h"

namespace Metal {
    PipelineInstance *GlobalIlluminationPass::getPipeline() {
        return context.corePipelines.giComputePipeline;
    }

    void GlobalIlluminationPass::clearTexture(const VkImage &image) const {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
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
        vkCmdClearColorImage(vkCommandBuffer, image, layout, &color, 1,
                             &imageSubresourceRange);
    }

    void GlobalIlluminationPass::changeTextureModeEnd() const {
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

    void GlobalIlluminationPass::changeTextureModeBegin() const {
        VkImageMemoryBarrier transferToGeneral = ImageUtils::TransferDstToGeneralBarrier(
            context.coreTextures.globalIllumination->vkImage);

        vkCmdPipelineBarrier(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &transferToGeneral);
    }

    void GlobalIlluminationPass::onSync() {
        if (isFirstRun || context.engineContext.isCameraUpdated() || context.engineContext.isGISettingsUpdated() ||
            context.engineContext.isLightingDataUpdated()) {
            clearTexture(context.coreTextures.globalIllumination->vkImage);
            clearTexture(context.coreTextures.giSurfaceCache->vkImage);
            context.engineContext.resetGiAccumulationCount();
            isFirstRun = false;
        }

        if (!context.engineRepository.giEnabled) {
            return;
        }

        changeTextureModeBegin();

        recordImageDispatch(context.coreTextures.globalIllumination, 8, 8);

        changeTextureModeEnd();
    }
} // Metal
