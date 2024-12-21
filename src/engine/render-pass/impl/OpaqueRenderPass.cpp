#include "OpaqueRenderPass.h"

#include "../../../context/ApplicationContext.h"
#include "../../../context/repository/CorePipelines.h"
#include "../../enum/LevelOfDetail.h"
#include "../../service/world/components/MeshComponent.h"
#include "../../service/world/components/TransformComponent.h"

namespace Metal {
    void OpaqueRenderPass::onSync() {
        for (const auto &val: worldRepository.meshes | std::views::values) {
            if (const auto *mesh = val; !mesh->meshId.empty()) {
                if (const auto *r = streamingRepository.streamMesh(mesh->meshId, LevelOfDetail::LOD_0); r != nullptr) {
                    mPushConstant.model = worldRepository.transforms[mesh->getEntityId()]->model;
                    recordPushConstant(&mPushConstant);
                    drawMesh(r);
                }
            }
        }
    }

    PipelineInstance *OpaqueRenderPass::getPipeline() {
        return context.vulkanContext.corePipelines.gBufferPipeline;
    }
} // Metal
