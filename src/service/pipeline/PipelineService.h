#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
#include <vector>

#include "../abstract/AbstractResourceService.h"

namespace Metal {
    struct PipelineBuilder;
    struct FrameBufferInstance;
    struct PipelineInstance;
    struct DescriptorInstance;

    class PipelineService final : public AbstractResourceService {
        PipelineInstance *createComputePipeline(const PipelineBuilder &pipelineBuilder) const;

        PipelineInstance *createRenderingPipeline(PipelineBuilder &pipelineBuilder);

        PipelineInstance *createRayTracingPipeline(const PipelineBuilder &pipelineBuilder) const;

        void createPipelineLayout(const std::vector<DescriptorInstance *> &descriptorSetsToBind,
                                  unsigned int pushConstantsSize,
                                  PipelineInstance *pipeline) const;

    public:
        explicit PipelineService()
            : AbstractResourceService() {
        }

        PipelineInstance *createPipeline(PipelineBuilder &pipelineBuilder);
    };
} // Metal

#endif //PIPELINESERVICE_H
