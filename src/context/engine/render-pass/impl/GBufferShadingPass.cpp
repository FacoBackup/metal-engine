#include "GBufferShadingPass.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void GBufferShadingPass::onInitialize() {
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
        pipelineInstance = context.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void GBufferShadingPass::onSync() {
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
