#ifndef COREPIPELINES_H
#define COREPIPELINES_H
#include "../abstract/AbstractCoreRepository.h"


namespace Metal {
    struct PipelineInstance;
    struct MeshInstance;

    struct CorePipelines final : AbstractCoreRepository {
        explicit CorePipelines(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;

        void dispose() const;

        PipelineInstance *gBufferPipeline = nullptr;
        PipelineInstance *gridPipeline = nullptr;
        PipelineInstance *iconPipeline = nullptr;
        PipelineInstance *postProcessingPipeline = nullptr;
        PipelineInstance *upScalingPipeline = nullptr;
        PipelineInstance *shadingPipeline = nullptr;
        PipelineInstance *atmospherePipeline = nullptr;
        PipelineInstance *giComputePipeline = nullptr;
        PipelineInstance *voxelDebugVisualizerPipeline = nullptr;
    };
} // Metal

#endif //CORESHADERS_H
