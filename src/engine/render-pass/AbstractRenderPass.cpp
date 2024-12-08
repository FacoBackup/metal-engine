#include "AbstractRenderPass.h"
#include "../../common/runtime/ApplicationContext.h"
#include "../service/core/pipeline/PipelineInstance.h"
#include "../service/core/pools/CommandBufferInstance.h"

namespace Metal {
    AbstractRenderPass::AbstractRenderPass(ApplicationContext &context): AbstractRuntimeComponent(context),
                                                                         buffers(
                                                                             context.getEngineContext().coreBuffers),
                                                                         pipelines(
                                                                             context.getEngineContext().corePipelines),
                                                                         frameBuffers(
                                                                             context.getEngineContext().
                                                                             coreFrameBuffers) {
    }

    void AbstractRenderPass::BindPipeline(const PipelineInstance *pipeline) {
        vkCmdBindPipeline(pipeline->commandBuffer->vkBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipeline);

        if (pipeline->descriptorSet != VK_NULL_HANDLE) {
            vkCmdBindDescriptorSets(pipeline->commandBuffer->vkBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipelineLayout,
                                    0,  // firstSet
                                    1,  // descriptorSetCount
                                    &pipeline->descriptorSet,
                                    0,	// dynamicOffsetCount
                                    nullptr);	// pDynamicOffsets
        }
    }
} // Metal
