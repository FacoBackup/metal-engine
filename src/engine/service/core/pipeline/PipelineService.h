#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
#include "../../AbstractResourceService.h"

namespace Metal {
    struct FrameBufferInstance;
    struct PipelineInstance;

    class PipelineService final : public AbstractResourceService {
    public:
        explicit PipelineService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        PipelineInstance *createRenderingPipeline(FrameBufferInstance *frameBuffer, const char *vertexShader, const char *fragmentShader) const;
    };
} // Metal

#endif //PIPELINESERVICE_H
