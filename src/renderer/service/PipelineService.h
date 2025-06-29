#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
#include <vector>

#include "../../common/AbstractResourceService.h"

namespace Metal {
    struct PipelineBuilder;
    struct FrameBufferInstance;
    struct PipelineInstance;
    struct DescriptorInstance;

    class PipelineService final : public AbstractResourceService {
        PipelineInstance *createComputePipeline(const PipelineBuilder &pipelineBuilder) const;

        PipelineInstance *createRenderingPipeline(PipelineBuilder &pipelineBuilder);

        void createPipelineLayout(const std::vector<DescriptorInstance *> &descriptorSetsToBind,
                                  unsigned int  pushConstantsSize,
                                  PipelineInstance *pipeline) const;

    public:

        PipelineInstance *createPipeline(PipelineBuilder &pipelineBuilder);
    };
} // Metal

#endif //PIPELINESERVICE_H
