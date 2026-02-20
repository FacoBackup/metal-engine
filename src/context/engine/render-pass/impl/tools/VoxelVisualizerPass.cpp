#include "VoxelVisualizerPass.h"
#include "../../../../ApplicationContext.h"

#include "../../../../../service/pipeline/PipelineBuilder.h"

namespace Metal {
    void VoxelVisualizerPass::onInitialize() {
        PipelineBuilder voxelVisualizerPipelineBuilder = PipelineBuilder::Of(
                    ApplicationContext::Get().coreFrameBuffers.postProcessingFBO,
                    "QUAD.vert",
                    "tools/VoxelDebugVisualizer.frag"
                )
                .setPushConstantsSize(sizeof(VoxelDebugSettingsPushConstant))
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.globalDataDescriptor.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.svoData.get())
                .addDescriptorSet(ApplicationContext::Get().coreDescriptorSets.surfaceCacheFragment.get());
        pipelineInstance = ApplicationContext::Get().pipelineService.createPipeline(voxelVisualizerPipelineBuilder);
    }

    bool VoxelVisualizerPass::shouldRun() {
    return ApplicationContext::Get().editorRepository.showRaySearchCountVoxels || ApplicationContext::Get().editorRepository.showRayTestCountVoxels;
    }

    void VoxelVisualizerPass::onSync() {
        settings.voxelDebugFlag = ApplicationContext::Get().editorRepository.shadingMode;
        settings.searchCountDivisor = ApplicationContext::Get().editorRepository.voxelSearchCount;
        settings.showRaySearchCount = ApplicationContext::Get().editorRepository.showRaySearchCountVoxels;
        settings.showRayTestCount = ApplicationContext::Get().editorRepository.showRayTestCountVoxels;
        recordPushConstant(&settings);
        recordDrawSimpleInstanced(3, 1);
    }
} // Metal
