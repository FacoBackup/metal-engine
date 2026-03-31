#include "CameraVisualizationPass.h"
#include "engine/dto/TransformComponent.h"
#include "engine/dto/CameraComponent.h"
#include "engine/dto/PipelineBuilder.h"
#include "engine/service/PipelineService.h"
#include "../repository/EditorRepository.h"
#include "engine/repository/WorldRepository.h"
#include "../enum/EngineResourceIDs.h"

namespace Metal {
    void CameraVisualizationPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                   getScopedResourceId(RID_POST_PROCESSING_RT),
                    "tools/CameraVisualization.vert",
                    "tools/CameraVisualization.frag"
                )
                .enablePrimitiveRendering()
                .enableDepthTest()
                .useTriangleStrip()
                .setCullMode(VK_CULL_MODE_NONE)
                .setPushConstantsSize(sizeof(CameraVisualizationPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA));
        pipelineInstance = pipelineService->createPipeline(builder);
    }

    bool CameraVisualizationPass::shouldRun() {
        return !editorRepository->isPlaying;
    }

    void CameraVisualizationPass::onSync() {
        auto view = worldRepository->registry.view<CameraComponent, TransformComponent>();
        for (auto entityId : view) {
            if (worldRepository->hiddenEntities.contains(entityId)) {
                continue;
            }

            const auto &camera = view.get<CameraComponent>(entityId);
            const auto &transform = view.get<TransformComponent>(entityId);

            pushConstant.model = transform.model;
            pushConstant.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow for cameras
            pushConstant.fov = camera.fov;
            pushConstant.zNear = camera.zNear;
            pushConstant.zFar = camera.zFar;
            pushConstant.aspectRatio = camera.aspectRatio;

            recordPushConstant(&pushConstant);
            // 16 vertices to draw 8 lines of the wireframe.
            recordDrawSimpleInstanced(16, 1);
        }
    }
} // Metal
