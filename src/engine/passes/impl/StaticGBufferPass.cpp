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
    void StaticGBufferPass::setup(RGPassBuilder &builder) {
        gBufferHandle = builder.getResourceHandle(RID_GBUFFER_RT);
        globalDataHandle = builder.getResourceHandle(RID_GLOBAL_DATA);
        materialDataHandle = builder.getResourceHandle(RID_MATERIAL_DATA_BUFFER);
        primitiveDataHandle = builder.getResourceHandle(RID_PRIMITIVE_DATA_BUFFER);

        builder.write(gBufferHandle, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        builder.read(globalDataHandle, VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_ACCESS_UNIFORM_READ_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
        builder.read(materialDataHandle, VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        builder.read(primitiveDataHandle, VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
    }

    void StaticGBufferPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    gBufferHandle,
                    "gbuffer/GBufferGen.vert",
                    "gbuffer/GBufferGen.frag"
                )
                .enablePrimitiveRendering()
                .setCullMode(VK_CULL_MODE_BACK_BIT)
                .enableDepthTest()
                .setPushConstantsSize(sizeof(StaticGeometryPushConstant))
                .addBufferBinding(globalDataHandle)
                .addBufferBinding(materialDataHandle)
                .addBufferBinding(primitiveDataHandle)
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

                recordPushConstant(&pushConstant);
                recordDrawMesh(meshInstance);
            });
    }
} // Metal
