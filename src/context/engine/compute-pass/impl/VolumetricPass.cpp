#include "./VolumetricPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void VolumetricPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("rt/VolumePathTracer.comp")
                .addDescriptorSet(context.coreDescriptorSets.rtDescriptorSet.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsBuffer.get())
                .addDescriptorSet(context.coreDescriptorSets.volumesBuffer.get())
                .addDescriptorSet(context.coreDescriptorSets.volumetricCurrentFrame.get());

        pipelineInstance = context.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void VolumetricPass::onSync() {
        clearTexture(context.coreTextures.volumetricCurrentFrame->vkImage);
        startWriting(context.coreTextures.volumetricCurrentFrame->vkImage);
        recordImageDispatch(context.coreTextures.volumetricCurrentFrame, 8, 8);
    }
} // Metal
