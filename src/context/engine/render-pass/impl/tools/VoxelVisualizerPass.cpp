#include "VoxelVisualizerPass.h"
#include "../../../../ApplicationContext.h"

#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void VoxelVisualizerPass::onInitialize() {
        PipelineBuilder voxelVisualizerPipelineBuilder = PipelineBuilder::Of(
                    context.coreFrameBuffers.shadingFBO,
                    "QUAD.vert",
                    "tools/VoxelDebugVisualizer.frag"
                )
                .setPushConstantsSize(sizeof(VoxelDebugSettingsPushConstant))
                .addDescriptorSet(context.coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(context.coreDescriptorSets.svoData.get())
                .addDescriptorSet(context.coreDescriptorSets.surfaceCacheFragment.get());
        pipelineInstance = context.pipelineService.createPipeline(voxelVisualizerPipelineBuilder);
    }

    bool VoxelVisualizerPass::shouldRun() {
        return context.editorRepository.showVoxels;
    }

    void VoxelVisualizerPass::onSync() {
        settings.voxelDebugFlag = context.editorRepository.shadingMode;
        settings.searchCountDivisor = context.editorRepository.voxelSearchCount;
        settings.showRaySearchCount = context.editorRepository.showRaySearchCountVoxels;
        settings.showRayTestCount = context.editorRepository.showRayTestCountVoxels;
        recordPushConstant(&settings);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
