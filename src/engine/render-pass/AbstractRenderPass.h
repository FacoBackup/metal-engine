#ifndef ABSTRACTRENDERPASS_H
#define ABSTRACTRENDERPASS_H

#include "../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    struct PipelineInstance;
}

namespace Metal {
    struct VulkanFrameData;
    struct CoreFrameBuffers;
    struct CorePipelines;
    struct CoreBuffers;

    class AbstractRenderPass : public AbstractRuntimeComponent {
    protected:
        CoreBuffers &buffers;
        CorePipelines &pipelines;
        CoreFrameBuffers &frameBuffers;

    public:
        explicit AbstractRenderPass(ApplicationContext &context);

        static void BindPipeline(const PipelineInstance *pipeline);

        virtual void render(VulkanFrameData &frameData) {
        }
    };
}

#endif
