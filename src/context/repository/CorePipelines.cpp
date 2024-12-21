#include "CorePipelines.h"

#include "../buffers/MeshPushConstant.h"
#include "../buffers/GBufferShadingPushConstant.h"
#include "../ApplicationContext.h"
#include "../runtime/PipelineInstance.h"
#include "../service//PipelineService.h"
#include "../service/PipelineBuilder.h"

namespace Metal {
    void CorePipelines::onInitialize() {
        PipelineBuilder gBufferPipelineBuilder = PipelineBuilder::Of(
                    context.vulkanContext.coreFrameBuffers.gBufferFBO,
                    "../resources/shaders/DEBUG.vert",
                    "../resources/shaders/DEBUG.frag"
                )
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.globalDataDescriptor.get())
                .setPrepareForMesh()
                .setDepthTest()
                .setCullMode(VK_CULL_MODE_BACK_BIT)
                .setPushConstantsSize(sizeof(MeshPushConstant));
        gBufferPipeline = pipelineService.createRenderingPipeline(gBufferPipelineBuilder);

        PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                    context.vulkanContext.coreFrameBuffers.auxFBO,
                    "../resources/shaders/QUAD.vert",
                    "../resources/shaders/GRID.frag"
                )
                .setBlendEnabled()
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.globalDataDescriptor.get());
        gridPipeline = pipelineService.createRenderingPipeline(gridPipelineBuilder);

        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of(
                    context.vulkanContext.coreFrameBuffers.postProcessingFBO,
                    "../resources/shaders/QUAD.vert",
                    "../resources/shaders/LENS_POST_PROCESSING.frag"
                )
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.postProcessingDescriptor.get());
        postProcessingPipeline = pipelineService.createRenderingPipeline(ppPipelineBuilder);


        PipelineBuilder gBufferShadingPipelineBuilder = PipelineBuilder::Of(
                    context.vulkanContext.coreFrameBuffers.auxFBO,
                    "../resources/shaders/QUAD.vert",
                    "../resources/shaders/GBufferShading.frag"
                )
                .setPushConstantsSize(sizeof(GBufferShadingPushConstant))
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.gBufferShadingDescriptor.get());
        gBufferShadingPipeline = pipelineService.createRenderingPipeline(gBufferShadingPipelineBuilder);
    }

    void CorePipelines::dispose() const {
        // TODO - Add button to UI to re-create pipelines and reload shaders
        gBufferPipeline->dispose(vulkanContext);
        gridPipeline->dispose(vulkanContext);
        postProcessingPipeline->dispose(vulkanContext);
    }
} // Metal
