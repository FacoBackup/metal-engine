#include "GridPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void GridPass::onInitialize() {
        PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "tools/Grid.frag"
                )
                .setBlendEnabled()
                .setPushConstantsSize(sizeof(GridPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.gBufferPosition.get());
        pipelineInstance = context.pipelineService.createPipeline(gridPipelineBuilder);
    }

    bool GridPass::shouldRun() {
        return context.isDebugMode() && context.editorRepository.showGrid;
    }

    void GridPass::onSync() {
        pushConstant.scale = context.editorRepository.gridScale;
        pushConstant.overlayObjects = context.editorRepository.gridOverlayObjects;
        pushConstant.threshold = context.editorRepository.gridThreshold;
        pushConstant.thickness = context.editorRepository.gridThickness;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
