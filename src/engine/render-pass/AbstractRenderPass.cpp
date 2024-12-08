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

} // Metal
