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

        PipelineInstance *debugPipeline = nullptr;
        PipelineInstance *gridPipeline = nullptr;
    };
} // Metal

#endif //CORESHADERS_H
