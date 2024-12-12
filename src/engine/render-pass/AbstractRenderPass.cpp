#include "AbstractRenderPass.h"
#include "../../context/ApplicationContext.h"
#include "../../context/runtime/PipelineInstance.h"

namespace Metal {
    AbstractRenderPass::AbstractRenderPass(ApplicationContext &context): AbstractRuntimeComponent(context),
                                                                         buffers(
                                                                             context.getVulkanContext().coreBuffers),
                                                                         pipelines(
                                                                             context.getVulkanContext().corePipelines),
                                                                         frameBuffers(
                                                                             context.getVulkanContext().
                                                                             coreFrameBuffers) {
    }

    void AbstractRenderPass::registerCommandBuffer(PipelineInstance *pipeline) {
        context.getVulkanContext().getFrameData().commandBuffers.push_back(pipeline->vkCommandBuffer);
    }

} // Metal
