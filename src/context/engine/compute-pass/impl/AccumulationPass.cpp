#include "AccumulationPass.h"
#include "../../../ApplicationContext.h"
#include "../../../../service/pipeline/PipelineBuilder.h"
#include "../../../../service/texture/TextureInstance.h"

namespace Metal {
    void AccumulationPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("PathTracerAccumulation.comp")
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get())
                .addDescriptorSet(context.coreDescriptorSets.currentImageCompute.get())
                .addDescriptorSet(context.coreDescriptorSets.diSurfaceCacheCompute.get())
                .addDescriptorSet(context.coreDescriptorSets.diSurfaceCacheComputeVisibility.get());
        pipelineInstance = context.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    bool AccumulationPass::shouldRun() {
        return context.engineRepository.enabledDenoiser;
    }

    void AccumulationPass::onSync() {
        clearTexture(context.coreTextures.diSurfaceCacheImageVisibility->vkImage);
        recordImageDispatch(context.coreTextures.currentFrame, 8, 8);
        endWriting(context.coreTextures.currentFrame->vkImage);
    }
} // Metal
