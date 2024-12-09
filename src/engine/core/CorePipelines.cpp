#include "CorePipelines.h"

#include "../../common/runtime/ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../service/core/pipeline/PipelineInstance.h"
#include "../service/core/pipeline/PipelineService.h"
#include "../service/core/command/CommandBufferInstance.h"

namespace Metal {
    void CorePipelines::onInitialize() {
        debugPipeline = pipelineService.createRenderingPipeline(
            context.getEngineContext().coreFrameBuffers.auxRenderPass, "resources/shaders/QUAD.vert", "resources/shaders/DEBUG.frag");

        debugPipeline->commandBuffer->startMapping();

        vkCmdDraw(debugPipeline->commandBuffer->vkCommandBuffer, 4, 1, 0, 0);

        debugPipeline->commandBuffer->stopMapping();

    }
} // Metal
