#include "CorePipelines.h"
#include "../../context/ApplicationContext.h"

#include "../../dto/push-constant/MeshPushConstant.h"
#include "../../dto/push-constant/GBufferShadingPushConstant.h"
#include "../../dto/push-constant/VoxelDebugSettingsPushConstant.h"
#include "../../service/pipeline/PipelineInstance.h"
#include "../../service/pipeline/PipelineService.h"
#include "../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void CorePipelines::onInitialize() {
        PipelineBuilder gBufferPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.gBufferFBO,
                    "GBufferGen.vert",
                    "GBufferGen.frag"
                )
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .setPrepareForMesh()
                .setDepthTest()
                .setCullMode(VK_CULL_MODE_BACK_BIT)
                .setPushConstantsSize(sizeof(MeshPushConstant));
        gBufferPipeline = pipelineService.createRenderingPipeline(gBufferPipelineBuilder);

        PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.auxFBO,
                    "QUAD.vert",
                    "Grid.frag"
                )
                .setBlendEnabled()
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get());
        gridPipeline = pipelineService.createRenderingPipeline(gridPipelineBuilder);

        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "PostProcessing.frag"
                )
                .addDescriptorSet(context.coreDescriptorSets.postProcessingDescriptor.get());
        postProcessingPipeline = pipelineService.createRenderingPipeline(ppPipelineBuilder);


        PipelineBuilder gBufferShadingPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.auxFBO,
                    "QUAD.vert",
                    "GBufferShading.frag"
                )
                .setPushConstantsSize(sizeof(GBufferShadingPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferShadingDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferShadingDescriptor1.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferShadingDescriptor2.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferShadingDescriptor3.get())
                .addDescriptorSet(context.coreDescriptorSets.brdfDescriptor.get());
        gBufferShadingPipeline = pipelineService.createRenderingPipeline(gBufferShadingPipelineBuilder);

        PipelineBuilder voxelVisualizerPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.auxFBO,
                    "QUAD.vert",
                    "VoxelDebugVisualizer.frag"
                )
                .setPushConstantsSize(sizeof(VoxelDebugSettingsPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.svoData.get());
        voxelDebugVisualizerPipeline = pipelineService.createRenderingPipeline(voxelVisualizerPipelineBuilder);
    }

    void CorePipelines::dispose() const {
        // TODO - Add button to UI to re-create pipelines and reload shaders
        gBufferPipeline->dispose(vulkanContext);
        gridPipeline->dispose(vulkanContext);
        gBufferShadingPipeline->dispose(vulkanContext);
        postProcessingPipeline->dispose(vulkanContext);
        voxelDebugVisualizerPipeline->dispose(vulkanContext);
    }
} // Metal
