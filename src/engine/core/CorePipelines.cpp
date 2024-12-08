#include "CorePipelines.h"

#include "../../common/runtime/ApplicationContext.h"
#include "../service/core/pipeline/PipelineInstance.h"
#include "../service/core/pipeline/PipelineService.h"
#include "../service/core/pools/CommandBufferInstance.h"

namespace Metal {
    void CorePipelines::onInitialize() {
        debugPipeline = pipelineService.createRenderingPipeline(
            context.getEngineContext().coreFrameBuffers.auxRenderPass, "shaders/QUAD.vert", "shaders/DEBUG.frag");
        debugPipeline->commandBuffer->startMapping();
        vkCmdBindPipeline(debugPipeline->commandBuffer->vkBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          debugPipeline->vkPipeline);
        vkCmdDraw(debugPipeline->commandBuffer->vkBuffer, 3, 1, 0, 0);
        debugPipeline->commandBuffer->stopMapping();

    }
} // Metal
