#include "VoxelVisualizerPass.h"
#include "../../../../ApplicationContext.h"

#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void VoxelVisualizerPass::onInitialize() {
        PipelineBuilder voxelVisualizerPipelineBuilder = PipelineBuilder::Of(
                    CTX.coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "tools/VoxelDebugVisualizer.frag"
                )
                .setPushConstantsSize(sizeof(VoxelDebugSettingsPushConstant))
                .addDescriptorSet(CTX.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(CTX.coreDescriptorSets.svoData.get())
                .addDescriptorSet(CTX.coreDescriptorSets.surfaceCacheFragment.get());
        pipelineInstance = CTX.pipelineService.createPipeline(voxelVisualizerPipelineBuilder);
    }

    bool VoxelVisualizerPass::shouldRun() {
    return CTX.editorRepository.showRaySearchCountVoxels || CTX.editorRepository.showRayTestCountVoxels;
    }

    void VoxelVisualizerPass::onSync() {
        settings.voxelDebugFlag = CTX.editorRepository.shadingMode;
        settings.searchCountDivisor = CTX.editorRepository.voxelSearchCount;
        settings.showRaySearchCount = CTX.editorRepository.showRaySearchCountVoxels;
        settings.showRayTestCount = CTX.editorRepository.showRayTestCountVoxels;
        recordPushConstant(&settings);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
