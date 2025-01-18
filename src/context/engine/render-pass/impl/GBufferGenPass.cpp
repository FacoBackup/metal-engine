#include "GBufferGenPass.h"

#include "../../../../context/ApplicationContext.h"
#include "../../../../enum/LevelOfDetail.h"
#include "../../../../repository/world/components/MeshComponent.h"
#include "../../../../repository/world/components/TransformComponent.h"
#include "../../../../service//framebuffer/FrameBufferInstance.h"

#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void GBufferGenPass::onInitialize() {
        PipelineBuilder gBufferPipelineBuilder = PipelineBuilder::Of(
                        context.coreFrameBuffers.gBufferFBO,
                        "GBufferGen.vert",
                        "GBufferGen.frag"
                    )
                    .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                    .addDescriptorSet(context.coreDescriptorSets.materialAlbedo.get())
                    .addDescriptorSet(context.coreDescriptorSets.materialNormal.get())
                    .addDescriptorSet(context.coreDescriptorSets.materialRoughness.get())
                    .addDescriptorSet(context.coreDescriptorSets.materialMetallic.get())
                    .addDescriptorSet(context.coreDescriptorSets.materialAO.get())
                    .addDescriptorSet(context.coreDescriptorSets.materialHeight.get())
                    .setPrepareForMesh()
                    .setDepthTest()
                    .setCullMode(VK_CULL_MODE_BACK_BIT)
                    .setPushConstantsSize(sizeof(MeshPushConstant));
        pipelineInstance = context.pipelineService.createPipeline(gBufferPipelineBuilder);
    }

    void GBufferGenPass::onSync() {
        unsigned int renderIndex = 0;
        for (const auto &pair: worldRepository.meshes) {
            if (const auto &mesh = pair.second; !mesh.meshId.empty()) {
                if (worldRepository.hiddenEntities.contains(mesh.getEntityId())) {
                    continue;
                }
                const auto *meshInstance = streamingRepository.streamMesh(mesh.meshId, LevelOfDetail::LOD_0);
                if (meshInstance != nullptr) {
                    const auto *materialInstance = streamingRepository.streamMaterial(mesh.materialId);

                    mPushConstant.model = worldRepository.transforms[mesh.getEntityId()].model;
                    mPushConstant.renderIndex = renderIndex;

                    mPushConstant.albedoEmissive = glm::vec4(mesh.albedoColor,
                                                             mesh.emissiveSurface);
                    mPushConstant.roughnessFactor = mesh.roughnessFactor;
                    mPushConstant.metallicFactor = mesh.metallicFactor;
                    mPushConstant.parallaxLayers = mesh.parallaxLayers;
                    mPushConstant.parallaxHeightScale = mesh.parallaxHeightScale;

                    if (materialInstance != nullptr) {
                        mPushConstant.useAlbedoTexture = materialInstance->useAlbedoTexture;
                        if (mPushConstant.useAlbedoTexture) {
                            bindSingleDescriptorSet(1, materialInstance->descriptorAlbedoTexture->vkDescriptorSet);
                        }
                        mPushConstant.useNormalTexture = materialInstance->useNormalTexture;
                        if (mPushConstant.useNormalTexture) {
                            bindSingleDescriptorSet(2, materialInstance->descriptorNormalTexture->vkDescriptorSet);
                        }
                        mPushConstant.useRoughnessTexture = materialInstance->useRoughnessTexture;
                        if (mPushConstant.useRoughnessTexture) {
                            bindSingleDescriptorSet(3, materialInstance->descriptorRoughnessTexture->vkDescriptorSet);
                        }
                        mPushConstant.useMetallicTexture = materialInstance->useMetallicTexture;
                        if (mPushConstant.useMetallicTexture) {
                            bindSingleDescriptorSet(4, materialInstance->descriptorMetallicTexture->vkDescriptorSet);
                        }
                        mPushConstant.useAOTexture = materialInstance->useAOTexture;
                        if (mPushConstant.useAOTexture) {
                            bindSingleDescriptorSet(5, materialInstance->descriptorAOTexture->vkDescriptorSet);
                        }
                        mPushConstant.useHeightTexture = materialInstance->useHeightTexture;
                        if (mPushConstant.useHeightTexture) {
                            bindSingleDescriptorSet(6, materialInstance->descriptorHeightTexture->vkDescriptorSet);
                        }
                    } else {
                        mPushConstant.useAlbedoTexture = false;
                        mPushConstant.useNormalTexture = false;
                        mPushConstant.useRoughnessTexture = false;
                        mPushConstant.useMetallicTexture = false;
                        mPushConstant.useAOTexture = false;
                        mPushConstant.useHeightTexture = false;
                    }

                    renderIndex++;
                    recordPushConstant(&mPushConstant);
                    recordDrawMesh(meshInstance);
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
} // Metal
