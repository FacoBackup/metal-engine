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
    public:
        PipelineInstance *createPipeline(PipelineBuilder &pipelineBuilder);

        void disposeResource(PipelineInstance *resource) override;

        std::vector<DescriptorInstance *> getAllDescriptors() const;

    private:
        PipelineInstance *createComputePipeline(const PipelineBuilder &pipelineBuilder, PipelineInstance *pipeline);

        PipelineInstance *createRenderingPipeline(PipelineBuilder &pipelineBuilder, PipelineInstance *pipeline);

        PipelineInstance *createRayTracingPipeline(const PipelineBuilder &pipelineBuilder, PipelineInstance *pipeline);

        void createPipelineLayout(
            unsigned int pushConstantsSize,
            PipelineInstance *pipeline) const;
    };
} // Metal

#endif //PIPELINESERVICE_H
