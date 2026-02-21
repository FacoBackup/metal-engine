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
            const auto entity = static_cast<entt::entity>(entityId);
            if (!worldRepository.registry.all_of<MeshComponent>(entity) || !worldRepository.registry.all_of<TransformComponent>(entity)) {
                continue;
            }

            const auto &mesh = worldRepository.registry.get<MeshComponent>(entity);
            if (mesh.meshId.empty()) {
                continue;
            }
            if (worldRepository.hiddenEntities.contains(entityId)) {
                continue;
            }

            const auto *meshInstance = streamingRepository.streamMesh(mesh.meshId, LevelOfDetail::LOD_0);
            if (!meshInstance) {
                continue;
            }

            pushConstant.model = worldRepository.registry.get<TransformComponent>(entity).model;
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
