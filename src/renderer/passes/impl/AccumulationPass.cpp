#include "AccumulationPass.h"
#include "../../../MetalContextProvider.h"
#include "../../data/PipelineBuilder.h"
#include "../../data/TextureInstance.h"

namespace Metal {
    void AccumulationPass::onInitialize() {
        PipelineBuilder shadingPipelineBuilder = PipelineBuilder::Of("PathTracerAccumulation.comp")
                .setPushConstantsSize(sizeof(AccumulationPC))
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.currentPositionsDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.current1FrameDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.previousFrameDescriptor.get());
        pipelineInstance = SINGLETONS.pipelineService.createPipeline(shadingPipelineBuilder);
    }

    void AccumulationPass::onSync() {
        if (initialized) {
            recordPushConstant(&pushConstant);
            recordImageDispatch(SINGLETONS.coreTextures.current1Frame, 8, 8);
        }
        initialized = true;
        pushConstant.previousFrameProjView = SINGLETONS.engineContext.getGlobalDataUBO().projView;
    }
} // Metal
