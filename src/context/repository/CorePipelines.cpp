#include "CorePipelines.h"

#include "MeshPushConstant.h"
#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/assets/MeshData.h"
#include "../runtime/PipelineInstance.h"
#include "../service//PipelineService.h"

namespace Metal {
    void CorePipelines::onInitialize() {
        gBufferPipeline = pipelineService.createRenderingPipeline(
            context.vulkanContext.coreFrameBuffers.gBufferFBO,
            VK_CULL_MODE_NONE,
            "../resources/shaders/DEBUG.vert",
            "../resources/shaders/DEBUG.frag",
            {context.vulkanContext.coreDescriptorSets.globalDataDescriptor.get()},
            sizeof(MeshPushConstant),
            false,
            true);

        gridPipeline = pipelineService.createRenderingPipeline(
            context.vulkanContext.coreFrameBuffers.auxFBO,
            VK_CULL_MODE_NONE,
            "../resources/shaders/QUAD.vert",
            "../resources/shaders/tool/GRID.frag",
            {context.vulkanContext.coreDescriptorSets.globalDataDescriptor.get()},
            0,
            true
        );

        postProcessingPipeline = pipelineService.createRenderingPipeline(
            context.vulkanContext.coreFrameBuffers.postProcessingFBO,
            VK_CULL_MODE_NONE,
            "../resources/shaders/QUAD.vert",
            "../resources/shaders/LENS_POST_PROCESSING.frag",
            {context.vulkanContext.coreDescriptorSets.postProcessingDescriptor.get()}
        );
    }

    void CorePipelines::dispose() const {
        // TODO - Add button to UI to re-create pipelines and reload shaders
        gBufferPipeline->dispose(vulkanContext);
        gridPipeline->dispose(vulkanContext);
        postProcessingPipeline->dispose(vulkanContext);
    }
} // Metal
