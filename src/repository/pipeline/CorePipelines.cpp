#include "CorePipelines.h"
#include "../../context/ApplicationContext.h"

#include "../../dto/push-constant/MeshPushConstant.h"
#include "../../dto/push-constant/GridPushConstant.h"
#include "../../dto/push-constant/GBufferShadingPushConstant.h"
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
                    .addDescriptorSet(context.coreDescriptorSets.gBufferMaterialD.get());
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
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferMaterialA.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferMaterialB.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferMaterialC.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferMaterialD.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get())
                .addDescriptorSet(context.coreDescriptorSets.globalIlluminationDescriptor.get());
        gBufferShadingPipeline = pipelineService.createPipeline(gBufferShadingPipelineBuilder);

        PipelineBuilder atmosphereBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.auxFBO,
                    "QUAD.vert",
                    "Atmosphere.frag"
                )
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferMaterialD.get());
        atmospherePipeline = pipelineService.createPipeline(atmosphereBuilder);


        PipelineBuilder giBuilder = PipelineBuilder::Of("GlobalIllumination.comp")
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.svoData.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferMaterialA.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferMaterialB.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferMaterialD.get())
                .addDescriptorSet(context.coreDescriptorSets.giComputeDescriptorA.get())
                .addDescriptorSet(context.coreDescriptorSets.giComputeDescriptorB.get());
        giComputePipeline = pipelineService.createPipeline(giBuilder);
    }

    void CorePipelines::dispose() const {
        gBufferPipeline->dispose(vulkanContext);
        gBufferShadingPipeline->dispose(vulkanContext);
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
