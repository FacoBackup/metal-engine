#ifndef COREPIPELINES_H
#define COREPIPELINES_H
#include "AbstractCoreRepository.h"

namespace Metal {
    struct PipelineInstance;

    struct CorePipelines final : AbstractCoreRepository {
        explicit CorePipelines(ApplicationContext &context)
            : AbstractCoreRepository(context) {
        }

        void onInitialize() override;

        PipelineInstance *debugPipeline = nullptr;
    };
} // Metal

#endif //CORESHADERS_H
