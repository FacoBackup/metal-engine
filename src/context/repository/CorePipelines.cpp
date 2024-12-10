#include "CorePipelines.h"

#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/PipelineInstance.h"
#include "../service//PipelineService.h"
#include "../runtime/CommandBufferInstance.h"

namespace Metal {
    void CorePipelines::onInitialize() {
        debugPipeline = pipelineService.createRenderingPipeline(
            context.getVulkanContext().coreFrameBuffers.auxRenderPass, "resources/shaders/QUAD.vert",
            "resources/shaders/DEBUG.frag", context.getVulkanContext().coreDescriptorSets.debugDescriptor);

        debugPipeline->commandBuffer->startMapping();
        vkCmdDraw(debugPipeline->commandBuffer->vkCommandBuffer, 3, 1, 0, 0);
        debugPipeline->commandBuffer->stopMapping();
    }
} // Metal
