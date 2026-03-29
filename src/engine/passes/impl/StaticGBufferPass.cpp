#include "StaticGBufferPass.h"
#include "engine/dto/TransformComponent.h"
#include "engine/dto/StaticGeometryComponent.h"
#include "engine/dto/PipelineBuilder.h"
#include "engine/service/PipelineService.h"
#include "engine/repository/WorldRepository.h"
#include "engine/service/MeshService.h"
#include "editor/enum/EngineResourceIDs.h"
#include "core/VulkanContext.h"

namespace Metal {
    void StaticGBufferPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    getScopedResourceId(RID_GBUFFER_RT),
                    "gbuffer/GBufferGen.vert",
                    "gbuffer/GBufferGen.frag"
                )
                .enablePrimitiveRendering()
                .setCullMode(VK_CULL_MODE_BACK_BIT)
                .enableDepthTest()
                .setPushConstantsSize(sizeof(StaticGeometryPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addBufferBinding(getScopedResourceId(RID_MATERIAL_DATA_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_PRIMITIVE_DATA_BUFFER))
                .addCombinedImageSamplerBinding(vulkanContext->vkImageSampler, VK_NULL_HANDLE,
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        pipelineInstance = pipelineService->createPipeline(builder);
    }

    void StaticGBufferPass::onSync() {
        worldRepository->registry.view<StaticGeometryComponent, TransformComponent>().each(
            [&](entt::entity entityId, const StaticGeometryComponent &mesh, const TransformComponent &transform) {
                if (mesh.meshId.empty()) {
                    return;
                }
                if (worldRepository->hiddenEntities.contains(entityId)) {
                    return;
                }

                const auto *meshInstance = meshService->stream(mesh.meshId);
                if (!meshInstance) {
                    return;
                }

                pushConstant.model = transform.model;
                pushConstant.renderIndex = mesh.renderIndex;
                pushConstant.parallaxHeightScale = mesh.parallaxScale;
                pushConstant.parallaxLayers = 32;

                recordPushConstant(&pushConstant);
                recordDrawMesh(meshInstance);
            });
    }
} // Metal
