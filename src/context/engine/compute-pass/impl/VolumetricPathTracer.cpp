#include "VolumetricPathTracer.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void VolumetricPathTracer::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("VolumetricPathTracer.comp")
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(context.coreDescriptorSets.currentFrameDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.volumesData.get())
                .addDescriptorSet(context.coreDescriptorSets.lightsData.get());
        pipelineInstance = context.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    bool VolumetricPathTracer::shouldRun() {
        return context.engineContext.getGlobalDataUBO().volumeCount > 0;
    }

    void VolumetricPathTracer::onSync() {
        recordImageDispatch(context.coreTextures.currentFrame, 8, 8);
    }
} // Metal