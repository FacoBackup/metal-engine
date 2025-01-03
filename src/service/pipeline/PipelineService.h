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
        PipelineInstance *createComputePipeline(const PipelineBuilder &pipelineBuilder);

        PipelineInstance *createRenderingPipeline(PipelineBuilder &pipelineBuilder);

        void createPipelineLayout(const std::vector<DescriptorInstance *> &descriptorSetsToBind,
                                  uint32_t pushConstantsSize,
                                  PipelineInstance *pipeline) const;

    public:
        explicit PipelineService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        PipelineInstance *createPipeline(PipelineBuilder &pipelineBuilder);
    };
} // Metal

#endif //PIPELINESERVICE_H
