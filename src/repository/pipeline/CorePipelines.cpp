#include "CorePipelines.h"
#include "../../context/ApplicationContext.h"

#include "../../dto/push-constant/MeshPushConstant.h"
#include "../../dto/push-constant/GridPushConstant.h"
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
        gBufferPipeline = pipelineService.createPipeline(gBufferPipelineBuilder);

        if (context.isDebugMode()) {
            PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                        context.coreFrameBuffers.auxFBO,
                        "QUAD.vert",
                        "tools/Grid.frag"
                    )
                    .setBlendEnabled()
                    .setPushConstantsSize(sizeof(GridPushConstant))
                    .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                    .addDescriptorSet(context.coreDescriptorSets.gBufferDepthIDUV.get());
            gridPipeline = pipelineService.createPipeline(gridPipelineBuilder);

            PipelineBuilder iconPipelineBuilder = PipelineBuilder::Of(
                        context.coreFrameBuffers.auxFBO,
                        "tools/Icon.vert",
                        "tools/Icon.frag"
                    )
                    .useTriangleStrip()
                    .setPushConstantsSize(sizeof(IconPushConstant))
                    .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                    .addDescriptorSet(context.coreDescriptorSets.iconsDescriptor.get());
            iconPipeline = pipelineService.createPipeline(iconPipelineBuilder);


            PipelineBuilder voxelVisualizerPipelineBuilder = PipelineBuilder::Of(
                        context.coreFrameBuffers.auxFBO,
                        "QUAD.vert",
                        "tools/VoxelDebugVisualizer.frag"
                    )
                    .setPushConstantsSize(sizeof(VoxelDebugSettingsPushConstant))
                    .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                    .addDescriptorSet(context.coreDescriptorSets.svoData.get());
            voxelDebugVisualizerPipeline = pipelineService.createPipeline(voxelVisualizerPipelineBuilder);
        }

        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "PostProcessing.frag"
                )
                .addDescriptorSet(context.coreDescriptorSets.postProcessingDescriptor.get());
        postProcessingPipeline = pipelineService.createPipeline(ppPipelineBuilder);


        PipelineBuilder gBufferShadingPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.auxFBO,
                    "QUAD.vert",
                    "GBufferShading.frag"
                )
                .setPushConstantsSize(sizeof(GBufferShadingPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferAlbedo.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferShadingRMAO.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferNormal.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferDepthIDUV.get())
                .addDescriptorSet(context.coreDescriptorSets.brdfDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.giDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get());
        gBufferShadingPipeline = pipelineService.createPipeline(gBufferShadingPipelineBuilder);

        PipelineBuilder atmosphereBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.auxFBO,
                    "QUAD.vert",
                    "Atmosphere.frag"
                )
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferDepthIDUV.get());
        atmospherePipeline = pipelineService.createPipeline(atmosphereBuilder);

        PipelineBuilder globalIlluminationPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.globalIlluminationFBO,
                    "QUAD.vert",
                    "GlobalIllumination.frag"
                )
                .setPushConstantsSize(sizeof(GIPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.svoData.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferNormal.get());

        globalIlluminationPipeline = pipelineService.createPipeline(globalIlluminationPipelineBuilder);


        PipelineBuilder giBuilder = PipelineBuilder::Of("GlobalIllumination.comp")
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.giComputeDescriptor.get());
        giComputePipeline = pipelineService.createPipeline(giBuilder);
    }

    void CorePipelines::dispose() const {
        gBufferPipeline->dispose(vulkanContext);
        gBufferShadingPipeline->dispose(vulkanContext);
        atmospherePipeline->dispose(vulkanContext);
        globalIlluminationPipeline->dispose(vulkanContext);
        postProcessingPipeline->dispose(vulkanContext);
        giComputePipeline->dispose(vulkanContext);
        if (context.isDebugMode()) {
            gridPipeline->dispose(vulkanContext);
            iconPipeline->dispose(vulkanContext);
            voxelDebugVisualizerPipeline->dispose(vulkanContext);
        }
    }
} // Metal
