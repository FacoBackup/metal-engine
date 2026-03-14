#ifndef SPATIALFILTERPASS_H
#define SPATIALFILTERPASS_H
#include "../AbstractComputePass.h"

namespace Metal {
    class PipelineService;

    class SpatialFilterPass final : public AbstractComputePass {
        PipelineService *pipelineService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"PipelineService", &pipelineService}
            };
        }

        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //SPATIALFILTERPASS_H
