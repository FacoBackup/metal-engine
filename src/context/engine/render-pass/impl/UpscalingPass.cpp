#include "UpscalingPass.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../service/framebuffer/FrameBufferInstance.h"

namespace Metal {
    void UpscalingPass::onSync() {
        // pushConstant.upscaleFactor = context.engineRepository.shadingResInvScale;
        // pushConstant.targetImageSize.x = context.coreFrameBuffers.compositionFBO->bufferWidth;
        // pushConstant.targetImageSize.y = context.coreFrameBuffers.compositionFBO->bufferHeight;
        // recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }

    PipelineInstance *UpscalingPass::getPipeline() {
        return context.corePipelines.upScalingPipeline;
    }
} // Metal
