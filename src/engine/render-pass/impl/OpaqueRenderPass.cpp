#include "OpaqueRenderPass.h"

#include <glm/ext/matrix_transform.hpp>

#include "../../../context/ApplicationContext.h"
#include "../../../context/repository/CorePipelines.h"
#include "../../../context/runtime/PipelineInstance.h"

namespace Metal {
    void OpaqueRenderPass::onSync() {
        const auto &pipelines = context.getVulkanContext().corePipelines;
        if (pipelines.sampleMesh == nullptr) return;
        mPushConstant.model = glm::identity<glm::mat4x4>();
        recordPushConstant(&mPushConstant);
        bindMesh(pipelines.sampleMesh);
    }

    PipelineInstance *OpaqueRenderPass::getPipeline() {
        return context.getVulkanContext().corePipelines.debugPipeline;
    }
} // Metal
