#include "GridPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void GridPass::onInitialize() {
        PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                    ApplicationContext::Get().coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "tools/Grid.frag"
                )
                .setBlendEnabled()
                .setPushConstantsSize(sizeof(GridPushConstant))
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.gBufferPosition.get());
        pipelineInstance = ApplicationContext::Get().pipelineService.createPipeline(gridPipelineBuilder);
    }

    bool GridPass::shouldRun() {
        return ApplicationContext::Get().isDebugMode() && ApplicationContext::Get().editorRepository.showGrid;
    }

    void GridPass::onSync() {
        pushConstant.scale = ApplicationContext::Get().editorRepository.gridScale;
        pushConstant.overlayObjects = ApplicationContext::Get().editorRepository.gridOverlayObjects;
        pushConstant.threshold = ApplicationContext::Get().editorRepository.gridThreshold;
        pushConstant.thickness = ApplicationContext::Get().editorRepository.gridThickness;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
