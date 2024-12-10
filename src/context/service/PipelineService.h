#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
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
                                                  DescriptorInstance *descriptor) const;
    };
} // Metal

#endif //PIPELINESERVICE_H
