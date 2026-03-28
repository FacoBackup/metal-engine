#include "FallbackPass.h"
#include "engine/dto/TransformComponent.h"
#include "engine/dto/PrimitiveComponent.h"
#include "engine/dto/PipelineBuilder.h"
#include "engine/service/PipelineService.h"
#include "../repository/EditorRepository.h"
#include "engine/repository/WorldRepository.h"
#include "engine/service/MeshService.h"
#include "../enum/EngineResourceIDs.h"
#include "../../core/VulkanContext.h"

namespace Metal {
    void FallbackPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    getScopedResourceId(RID_POST_PROCESSING_FBO),
                    "tools/debug_phong.vert",
                    "tools/debug_phong.frag"
                )
                .enablePrimitiveRendering()
                .enableDepthTest()
                .setPushConstantsSize(sizeof(DebugPhongPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addBufferBinding(getScopedResourceId(RID_MATERIAL_DATA_BUFFER))
                .addCombinedImageSamplerBinding(vulkanContext->vkImageSampler, VK_NULL_HANDLE,
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        pipelineInstance = pipelineService->createPipeline(builder);
    }

    bool FallbackPass::shouldRun() {
        return !editorRepository->isPlaying && !vulkanContext->isRayTracingSupported();
    }

    void FallbackPass::onSync() {
        worldRepository->registry.view<PrimitiveComponent, TransformComponent>().each(
            [&](entt::entity entityId, const PrimitiveComponent &mesh, const TransformComponent &transform) {
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
