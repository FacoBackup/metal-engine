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
                        mPushConstant.useAlbedoTexture = materialInstance->useAlbedoTexture ? 1 : 0;
                        if (mPushConstant.useAlbedoTexture) {
                            bindSingleDescriptorSet(1, materialInstance->descriptorAlbedoTexture->vkDescriptorSet);
                        }
                        mPushConstant.useNormalTexture = materialInstance->useNormalTexture ? 1 : 0;
                        if (mPushConstant.useNormalTexture) {
                            bindSingleDescriptorSet(2, materialInstance->descriptorNormalTexture->vkDescriptorSet);
                        }
                        mPushConstant.useRoughnessTexture = materialInstance->useRoughnessTexture ? 1 : 0;
                        if (mPushConstant.useRoughnessTexture) {
                            bindSingleDescriptorSet(3, materialInstance->descriptorRoughnessTexture->vkDescriptorSet);
                        }
                        mPushConstant.useMetallicTexture = materialInstance->useMetallicTexture ? 1 : 0;
                        if (mPushConstant.useMetallicTexture) {
                            bindSingleDescriptorSet(4, materialInstance->descriptorMetallicTexture->vkDescriptorSet);
                        }
                        mPushConstant.useHeightTexture = materialInstance->useHeightTexture ? 1 : 0;
                        if (mPushConstant.useHeightTexture) {
                            bindSingleDescriptorSet(5, materialInstance->descriptorHeightTexture->vkDescriptorSet);
                        }
                    } else {
                        mPushConstant.useAlbedoTexture = 0;
                        mPushConstant.useNormalTexture = 0;
                        mPushConstant.useRoughnessTexture = 0;
                        mPushConstant.useMetallicTexture = 0;
                        mPushConstant.useHeightTexture = 0;
                    }

                    renderIndex++;
                    recordPushConstant(&mPushConstant);
                    recordDrawMesh(meshInstance);
                }
            }
        }
    }
} // Metal
