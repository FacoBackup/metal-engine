#ifndef TEMPORALACCUMULATIONPASS_H
#define TEMPORALACCUMULATIONPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class PipelineService;

    class TemporalAccumulationPass final : public AbstractComputePass {
        PipelineService *pipelineService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"PipelineService", pipelineService}
            };
        }

        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //TEMPORALACCUMULATIONPASS_H
