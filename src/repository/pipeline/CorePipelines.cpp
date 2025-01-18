#include "CorePipelines.h"
#include "../../context/ApplicationContext.h"

#include "../../dto/push-constant/PostProcessingPushConstant.h"
#include "../../dto/push-constant/MeshPushConstant.h"
#include "../../dto/push-constant/GridPushConstant.h"
#include "../../dto/push-constant/VoxelDebugSettingsPushConstant.h"
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
                .addDescriptorSet(context.coreDescriptorSets.materialAlbedo.get())
                .addDescriptorSet(context.coreDescriptorSets.materialNormal.get())
                .addDescriptorSet(context.coreDescriptorSets.materialRoughness.get())
                .addDescriptorSet(context.coreDescriptorSets.materialMetallic.get())
                .addDescriptorSet(context.coreDescriptorSets.materialAO.get())
                .addDescriptorSet(context.coreDescriptorSets.materialHeight.get())
                .setPrepareForMesh()
                .setDepthTest()
                .setCullMode(VK_CULL_MODE_BACK_BIT)
                .setPushConstantsSize(sizeof(MeshPushConstant));
        gBufferPipeline = pipelineService.createPipeline(gBufferPipelineBuilder);

        if (context.isDebugMode()) {
            PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                        context.coreFrameBuffers.shadingFBO,
                        "QUAD.vert",
                        "tools/Grid.frag"
                    )
                    .setBlendEnabled()
                    .setPushConstantsSize(sizeof(GridPushConstant))
                    .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                    .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get());
            gridPipeline = pipelineService.createPipeline(gridPipelineBuilder);

            PipelineBuilder iconPipelineBuilder = PipelineBuilder::Of(
                        context.coreFrameBuffers.shadingFBO,
                        "QUAD.vert",
                        "tools/Icon.frag"
                    )
                    .setPushConstantsSize(sizeof(IconPushConstant))
                    .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                    .addDescriptorSet(context.coreDescriptorSets.lightsData.get());
            iconPipeline = pipelineService.createPipeline(iconPipelineBuilder);

            PipelineBuilder voxelVisualizerPipelineBuilder = PipelineBuilder::Of(
                        context.coreFrameBuffers.shadingFBO,
                        "QUAD.vert",
                        "tools/VoxelDebugVisualizer.frag"
                    )
                    .setPushConstantsSize(sizeof(VoxelDebugSettingsPushConstant))
                    .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                    .addDescriptorSet(context.coreDescriptorSets.svoData.get())
                    .addDescriptorSet(context.coreDescriptorSets.surfaceCacheFragment.get());
            voxelDebugVisualizerPipeline = pipelineService.createPipeline(voxelVisualizerPipelineBuilder);
        }

        PipelineBuilder ppPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "PostProcessing.frag"
                )
                .setPushConstantsSize(sizeof(PostProcessingPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.postProcessingDescriptor.get());
        postProcessingPipeline = pipelineService.createPipeline(ppPipelineBuilder);

        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.shadingFBO,
                    "QUAD.vert",
                    "GBufferShading.frag"
                )
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferAlbedo.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferNormal.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get())
                .addDescriptorSet(context.coreDescriptorSets.globalIlluminationDescriptor.get());
        shadingPipeline = pipelineService.createPipeline(shadingPipelineBuilder);

        PipelineBuilder atmosphereBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.shadingFBO,
                    "QUAD.vert",
                    "Atmosphere.frag"
                )
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get());
        atmospherePipeline = pipelineService.createPipeline(atmosphereBuilder);


        PipelineBuilder giBuilder = PipelineBuilder::Of("GlobalIllumination.comp")
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.svoData.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferAlbedo.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferNormal.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(context.coreDescriptorSets.surfaceCacheCompute.get())
                .addDescriptorSet(context.coreDescriptorSets.giCompute.get());
        giComputePipeline = pipelineService.createPipeline(giBuilder);
    }

    void CorePipelines::dispose() const {
        gBufferPipeline->dispose(vulkanContext);
        shadingPipeline->dispose(vulkanContext);
        atmospherePipeline->dispose(vulkanContext);
        postProcessingPipeline->dispose(vulkanContext);
        giComputePipeline->dispose(vulkanContext);
        if (context.isDebugMode()) {
            gridPipeline->dispose(vulkanContext);
            iconPipeline->dispose(vulkanContext);
            voxelDebugVisualizerPipeline->dispose(vulkanContext);
        }
    }
} // Metal
