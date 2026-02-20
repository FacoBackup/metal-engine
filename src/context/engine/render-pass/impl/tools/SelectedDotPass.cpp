#include "SelectedDotPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../enum/LevelOfDetail.h"
#include "../../../../../repository/world/components/TransformComponent.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void SelectedDotPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    CTX.coreFrameBuffers.postProcessingFBO,
                    "tools/SelectedDot.vert",
                    "tools/SelectedDot.frag"
                )
                .setBlendEnabled()
                .setPrepareForMesh()
                .setCullMode(VK_CULL_MODE_BACK_BIT)
                .setPushConstantsSize(sizeof(MeshPushConstant))
                .addDescriptorSet(CTX.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.gBufferPosition.get());
        pipelineInstance = CTX.pipelineService.createPipeline(builder);
    }

    bool SelectedDotPass::shouldRun() {
        return !CTX.editorRepository.selected.empty();
    }

    void SelectedDotPass::onSync() {
        for (const auto &pair: CTX.editorRepository.selected) {
            if (!pair.second) {
                continue;
            }
            const EntityID entityId = pair.first;
            if (!worldRepository.meshes.contains(entityId) || !worldRepository.transforms.contains(entityId)) {
                continue;
            }

            const auto &mesh = worldRepository.meshes[entityId];
            if (mesh.meshId.empty()) {
                continue;
            }
            if (worldRepository.hiddenEntities.contains(mesh.getEntityId())) {
                continue;
            }

            const auto *meshInstance = streamingRepository.streamMesh(mesh.meshId, LevelOfDetail::LOD_0);
            if (!meshInstance) {
                continue;
            }

            pushConstant.model = worldRepository.transforms[entityId].model;
            pushConstant.albedoEmissive.x = CTX.editorRepository.selectionColor.x;
            pushConstant.albedoEmissive.y = CTX.editorRepository.selectionColor.y;
            pushConstant.albedoEmissive.z = CTX.editorRepository.selectionColor.z;
            pushConstant.albedoEmissive.w = CTX.editorRepository.selectionOutlineThickness;
            pushConstant.renderIndex = mesh.renderIndex;
            pushConstant.roughnessFactor = 0;
            pushConstant.metallicFactor = 0;
            pushConstant.useAlbedoTexture = 0;
            pushConstant.useNormalTexture = 0;
            pushConstant.useRoughnessTexture = 0;
            pushConstant.useMetallicTexture = 0;
            pushConstant.useHeightTexture = 0;
            pushConstant.parallaxHeightScale = 0;
            pushConstant.parallaxLayers = 0;

            recordPushConstant(&pushConstant);
            recordDrawMesh(meshInstance);
        }
    }
} // Metal
