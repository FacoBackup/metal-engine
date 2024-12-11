#include "CorePipelines.h"

#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/PipelineInstance.h"
#include "../service//PipelineService.h"
#include "../runtime/CommandBufferInstance.h"

namespace Metal {
    void CorePipelines::onInitialize() {
        debugPipeline = pipelineService.createRenderingPipeline(
            context.getVulkanContext().coreFrameBuffers.auxRenderPass,
            VK_CULL_MODE_NONE,
            "../resources/shaders/QUAD.vert",
            "../resources/shaders/DEBUG.frag",
            {context.getVulkanContext().coreDescriptorSets.globalDataDescriptor},
            sizeof(glm::vec4));

        debugPipeline->commandBuffer->startRecording();
        glm::vec4 c = glm::vec4(0, 1, 1, 1.0f);
        debugPipeline->commandBuffer->recordPushConstant(&c);
        debugPipeline->commandBuffer->recordDrawSimpleInstanced(3, 1);
        debugPipeline->commandBuffer->stopRecording();
    }
} // Metal
