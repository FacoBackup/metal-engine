#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
#include <vector>

#include "../abstract/AbstractResourceService.h"
#include "PipelineInstance.h"

namespace Metal {
    struct PipelineBuilder;
    struct FrameBufferInstance;
    struct PipelineInstance;
    struct DescriptorInstance;

    class PipelineService final : public AbstractResourceService<PipelineInstance> {
        PipelineInstance *createComputePipeline(const PipelineBuilder &pipelineBuilder);

        PipelineInstance *createRenderingPipeline(PipelineBuilder &pipelineBuilder);

        PipelineInstance *createRayTracingPipeline(const PipelineBuilder &pipelineBuilder);

        void createPipelineLayout(const std::vector<DescriptorInstance *> &descriptorSetsToBind,
                                  unsigned int pushConstantsSize,
                                  PipelineInstance *pipeline) const;

    public:
        explicit PipelineService()
            : AbstractResourceService<PipelineInstance>() {
        }

        PipelineInstance *createPipeline(PipelineBuilder &pipelineBuilder);

        void disposeResource(PipelineInstance *resource) override;
    };
} // Metal

#endif //PIPELINESERVICE_H
