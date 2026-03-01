#include "GBufferGenPass.h"

#include "../../../../context/ApplicationContext.h"
#include "../../../../repository/world/components/MeshComponent.h"
#include "../../../../repository/world/components/TransformComponent.h"
#include "../../../../service//framebuffer/FrameBufferInstance.h"

#include "../../../../service/material/MaterialService.h"
#include "../../../../service/material/MaterialInstance.h"
#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void GBufferGenPass::onInitialize() {
        PipelineBuilder gBufferPipelineBuilder = PipelineBuilder::Of(
                    "gBufferFBO",
                    "GBufferGen.vert",
                    "GBufferGen.frag"
                )
                .addResourceBinding("globalData")
                .addResourceBinding("materialBuffer")
                .addResourceBinding(CTX.vulkanContext.vkImageSampler, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1000)
                .setPrepareForMesh()
                .setDepthTest()
                .setCullMode(VK_CULL_MODE_BACK_BIT)
                .setPushConstantsSize(sizeof(MeshPushConstant));
        pipelineInstance = CTX.pipelineService.createPipeline(gBufferPipelineBuilder);
    }

    void GBufferGenPass::onSync() {
        unsigned int renderIndex = 0;
        for (auto entity: worldRepository.registry.storage<entt::entity>()) {
            if (!worldRepository.registry.all_of<MeshComponent, TransformComponent>(entity)) continue;
            auto &mesh = worldRepository.registry.get<MeshComponent>(entity);
            if (!mesh.meshId.empty()) {
                if (worldRepository.hiddenEntities.contains(static_cast<EntityID>(entity))) {
                    continue;
                }
                const auto *meshInstance = streamingRepository.streamMesh(mesh.meshId);
                if (meshInstance != nullptr) {
                    mPushConstant.model = worldRepository.registry.get<TransformComponent>(entity).model;
                    mPushConstant.renderIndex = mesh.renderIndex = renderIndex;

                    mPushConstant.parallaxLayers = mesh.parallaxLayers;
                    mPushConstant.parallaxHeightScale = mesh.parallaxHeightScale;

                    mPushConstant.materialIndex = CTX.materialService.getMaterialIndex(mesh.materialId);

                    renderIndex++;
                    recordPushConstant(&mPushConstant);
                    recordDrawMesh(meshInstance);
                }
            }
        }
    }
} // Metal
