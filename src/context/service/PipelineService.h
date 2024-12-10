#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
#include <vector>

#include "../../common/interface/AbstractResourceService.h"

namespace Metal {
    struct FrameBufferInstance;
    struct PipelineInstance;
    struct DescriptorInstance;

    class PipelineService final : public AbstractResourceService {
    public:
        explicit PipelineService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        PipelineInstance *createRenderingPipeline(FrameBufferInstance *frameBuffer, const char *vertexShader,
                                                  const char *fragmentShader,
                                                  const std::vector<DescriptorInstance *> &descriptors,
                                                  uint32_t pushConstantsSize = 0) const;
    };
} // Metal

#endif //PIPELINESERVICE_H
