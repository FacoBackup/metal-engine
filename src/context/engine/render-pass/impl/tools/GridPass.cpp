#include "GridPass.h"

#include "../../../../../context/ApplicationContext.h"
#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void GridPass::onInitialize() {
        PipelineBuilder gridPipelineBuilder = PipelineBuilder::Of(
                    CTX.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "tools/Grid.frag"
                )
                .setBlendEnabled()
                .setPushConstantsSize(sizeof(GridPushConstant))
                .addResourceBinding(CTX.coreBuffers.globalData)
                .addResourceBinding(CTX.vulkanContext.vkImageSampler, CTX.coreFrameBuffers.gBufferFBO->attachments[2]->vkImageView);
        pipelineInstance = CTX.pipelineService.createPipeline(gridPipelineBuilder);
    }

    bool GridPass::shouldRun() {
        return CTX.isDebugMode() && CTX.editorRepository.showGrid;
    }

    void GridPass::onSync() {
        pushConstant.scale = CTX.editorRepository.gridScale;
        pushConstant.overlayObjects = CTX.editorRepository.gridOverlayObjects;
        pushConstant.threshold = CTX.editorRepository.gridThreshold;
        pushConstant.thickness = CTX.editorRepository.gridThickness;
        recordPushConstant(&pushConstant);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
