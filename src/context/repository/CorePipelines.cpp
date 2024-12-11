#include "CorePipelines.h"

#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/PipelineInstance.h"
#include "../service//PipelineService.h"

namespace Metal {
    void CorePipelines::onInitialize() {
        debugPipeline = pipelineService.createRenderingPipeline(
            context.getVulkanContext().coreFrameBuffers.auxRenderPass,
            VK_CULL_MODE_NONE,
            "../resources/shaders/QUAD.vert",
            "../resources/shaders/DEBUG.frag",
            {context.getVulkanContext().coreDescriptorSets.globalDataDescriptor},
            sizeof(glm::vec4));

        debugPipeline->startRecording();
        glm::vec4 c = glm::vec4(0, 1, 1, 1.0f);
        debugPipeline->recordPushConstant(&c);
        debugPipeline->recordDrawSimpleInstanced(3, 1);
        debugPipeline->stopRecording();
    }
} // Metal
