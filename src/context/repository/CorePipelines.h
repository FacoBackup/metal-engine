#ifndef COREPIPELINES_H
#define COREPIPELINES_H
#include "AbstractCoreRepository.h"

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
        PipelineInstance *postProcessingPipeline = nullptr;
    };
} // Metal

#endif //CORESHADERS_H
