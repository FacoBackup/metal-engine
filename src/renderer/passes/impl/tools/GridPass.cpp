#include "GridPass.h"

#include "../../../../MetalContextProvider.h"
#include "../../../data/PipelineBuilder.h"

namespace Metal {
    void GridPass::onInitialize() {
        PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                    SINGLETONS.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "tools/Grid.frag"
                )
                .setBlendEnabled()
                .setPushConstantsSize(sizeof(GridPC))
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(SINGLETONS.coreDescriptorSets.currentPositionsFragmentDescriptor.get());
        pipelineInstance = SINGLETONS.pipelineService.createPipeline(gridPipelineBuilder);
    }

    bool GridPass::shouldRun() {
        return ContextProvider::Get().isDebugMode() && SINGLETONS.editorRepository.showGrid;
    }

    void GridPass::onSync() {
        pushConstant.scale = SINGLETONS.editorRepository.gridScale;
        pushConstant.threshold = SINGLETONS.editorRepository.gridThreshold;
        pushConstant.thickness = SINGLETONS.editorRepository.gridThickness;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
