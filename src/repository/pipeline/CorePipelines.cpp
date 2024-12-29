#include "CorePipelines.h"
#include "../../context/ApplicationContext.h"

#include "../../dto/push-constant/MeshPushConstant.h"
#include "../../dto/push-constant/GBufferShadingPushConstant.h"
#include "../../dto/push-constant/VoxelDebugSettingsPushConstant.h"
#include "../../dto/push-constant/GIPushConstant.h"
#include "../../dto/push-constant/IconPushConstant.h"
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

        if (context.isDebugMode()) {
            PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                        context.coreFrameBuffers.auxFBO,
                        "QUAD.vert",
                        "tools/Grid.frag"
                    )
                    .setBlendEnabled()
                    .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get());
            gridPipeline = pipelineService.createRenderingPipeline(gridPipelineBuilder);

            PipelineBuilder iconPipelineBuilder = PipelineBuilder::Of(
                        context.coreFrameBuffers.auxFBO,
                        "tools/Icon.vert",
                        "tools/Icon.frag"
                    )
                    .useTriangleStrip()
                    .setPushConstantsSize(sizeof(IconPushConstant))
                    .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                    .addDescriptorSet(context.coreDescriptorSets.iconsDescriptor.get());
            iconPipeline = pipelineService.createRenderingPipeline(iconPipelineBuilder);


            PipelineBuilder voxelVisualizerPipelineBuilder = PipelineBuilder::Of(
                        context.coreFrameBuffers.auxFBO,
                        "QUAD.vert",
                        "tools/VoxelDebugVisualizer.frag"
                    )
                    .setPushConstantsSize(sizeof(VoxelDebugSettingsPushConstant))
                    .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                    .addDescriptorSet(context.coreDescriptorSets.svoData.get());
            voxelDebugVisualizerPipeline = pipelineService.createRenderingPipeline(voxelVisualizerPipelineBuilder);
        }

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
                .addDescriptorSet(context.coreDescriptorSets.brdfDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.aoDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get());
        gBufferShadingPipeline = pipelineService.createRenderingPipeline(gBufferShadingPipelineBuilder);

        PipelineBuilder voxelAOBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.aoFBO,
                    "QUAD.vert",
                    "VoxelAO.frag"
                )
                .setPushConstantsSize(sizeof(GIPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.svoData.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get());
        voxelAOPipeline = pipelineService.createRenderingPipeline(voxelAOBuilder);
    }

    void CorePipelines::dispose() const {
        gBufferPipeline->dispose(vulkanContext);
        gBufferShadingPipeline->dispose(vulkanContext);
        postProcessingPipeline->dispose(vulkanContext);
        if (context.isDebugMode()) {
            gridPipeline->dispose(vulkanContext);
            iconPipeline->dispose(vulkanContext);
            voxelDebugVisualizerPipeline->dispose(vulkanContext);
        }
    }
} // Metal
