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
