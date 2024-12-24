#include "OpaqueRenderPass.h"

#include "../../../../context/ApplicationContext.h"
#include "../../../../repository/pipeline/CorePipelines.h"
#include "../../../../enum/LevelOfDetail.h"
#include "../../../../repository/world/components/MeshComponent.h"
#include "../../../../repository/world/components/TransformComponent.h"
#include "../../../../service//framebuffer/FrameBufferInstance.h"

namespace Metal {
    void OpaqueRenderPass::onSync() {
        unsigned int renderIndex = 0;
        for (const auto &pair: worldRepository.meshes) {
            if (const auto &mesh = pair.second; !mesh.meshId.empty()) {
                if (const auto *r = streamingRepository.streamMesh(mesh.meshId, LevelOfDetail::LOD_0); r != nullptr) {
                    mPushConstant.model = worldRepository.transforms[mesh.getEntityId()].model;
                    mPushConstant.renderIndex = renderIndex;
                    renderIndex++;
                    recordPushConstant(&mPushConstant);
                    recordDrawMesh(r);
                }
            }
        }

        // std::array<VkImageMemoryBarrier, 5> imageMemoryBarriers{};
        //
        // for (uint32_t i = 0; i < 5; ++i) {
        //     imageMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        //     imageMemoryBarriers[i].srcAccessMask =
        //             context.coreFrameBuffers.gBufferFBO->attachments[i]->depth
        //                 ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
        //                 : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // Writes from GBuffer pass
        //     imageMemoryBarriers[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT; // Reads in shading pass
        //     imageMemoryBarriers[i].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //     imageMemoryBarriers[i].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        //     imageMemoryBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //     imageMemoryBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //     imageMemoryBarriers[i].image = context.coreFrameBuffers.gBufferFBO->attachments[i]->vkImage;
        //     // Your GBuffer image
        //     imageMemoryBarriers[i].subresourceRange.aspectMask =
        //             context.coreFrameBuffers.gBufferFBO->attachments[i]->depth
        //                 ? VK_IMAGE_ASPECT_DEPTH_BIT
        //                 : VK_IMAGE_ASPECT_COLOR_BIT;
        //     imageMemoryBarriers[i].subresourceRange.baseMipLevel = 0;
        //     imageMemoryBarriers[i].subresourceRange.levelCount = 1;
        //     imageMemoryBarriers[i].subresourceRange.baseArrayLayer = 0;
        //     imageMemoryBarriers[i].subresourceRange.layerCount = 1;
        // }
        //
        // // Submit the pipeline barrier
        // vkCmdPipelineBarrier(
        //     vkCommandBuffer,
        //     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        //     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        //     0,
        //     0, nullptr,
        //     0, nullptr,
        //     imageMemoryBarriers.size(), imageMemoryBarriers.data()
        // );
    }

    PipelineInstance *OpaqueRenderPass::getPipeline() {
        return context.corePipelines.gBufferPipeline;
    }
} // Metal
