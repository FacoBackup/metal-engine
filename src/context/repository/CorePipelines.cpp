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
                    "GBufferGen.vert",
                    "GBufferGen.frag"
                )
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.globalDataDescriptor.get())
                .setPrepareForMesh()
                .setDepthTest()
                .setCullMode(VK_CULL_MODE_BACK_BIT)
                .setPushConstantsSize(sizeof(MeshPushConstant));
        gBufferPipeline = pipelineService.createRenderingPipeline(gBufferPipelineBuilder);

        PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                    context.vulkanContext.coreFrameBuffers.auxFBO,
                    "QUAD.vert",
                    "Grid.frag"
                )
                .setBlendEnabled()
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.globalDataDescriptor.get());
        gridPipeline = pipelineService.createRenderingPipeline(gridPipelineBuilder);

        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of(
                    context.vulkanContext.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "PostProcessing.frag"
                )
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.postProcessingDescriptor.get());
        postProcessingPipeline = pipelineService.createRenderingPipeline(ppPipelineBuilder);


        PipelineBuilder gBufferShadingPipelineBuilder = PipelineBuilder::Of(
                    context.vulkanContext.coreFrameBuffers.auxFBO,
                    "QUAD.vert",
                    "GBufferShading.frag"
                )
                .setPushConstantsSize(sizeof(GBufferShadingPushConstant))
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.gBufferShadingDescriptor.get())
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.gBufferShadingDescriptor1.get())
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.gBufferShadingDescriptor2.get())
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.gBufferShadingDescriptor3.get())
                .addDescriptorSet(context.vulkanContext.coreDescriptorSets.brdfDescriptor.get());
        gBufferShadingPipeline = pipelineService.createRenderingPipeline(gBufferShadingPipelineBuilder);
    }

    void CorePipelines::dispose() const {
        // TODO - Add button to UI to re-create pipelines and reload shaders
        gBufferPipeline->dispose(vulkanContext);
        gridPipeline->dispose(vulkanContext);
        postProcessingPipeline->dispose(vulkanContext);
    }
} // Metal
