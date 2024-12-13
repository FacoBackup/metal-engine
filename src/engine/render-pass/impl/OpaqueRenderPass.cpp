#include "OpaqueRenderPass.h"
#include "../../../context/ApplicationContext.h"
#include "../../../context/runtime/PipelineInstance.h"

namespace Metal {
    void OpaqueRenderPass::onSync() {
        pipelines.debugPipeline->startRecording();
        mPushConstant.model = glm::identity<glm::mat4x4>();
    
        pipelines.debugPipeline->recordPushConstant(&mPushConstant);
        pipelines.debugPipeline->bindMesh(pipelines.sampleMesh);
        pipelines.debugPipeline->stopRecording();

        registerCommandBuffer(pipelines.debugPipeline);
    }
} // Metal
