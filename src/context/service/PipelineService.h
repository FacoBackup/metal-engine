#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
#include <vector>

#include "../../common/interface/AbstractResourceService.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct FrameBufferInstance;
    struct PipelineInstance;
    struct DescriptorInstance;

    class PipelineService final : public AbstractResourceService {
    public:
        explicit PipelineService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        void createPipelineLayout(const std::vector<DescriptorInstance *> &descriptorSetsToBind,
                                  uint32_t pushConstantsSize,
                                  PipelineInstance *pipeline) const;

        PipelineInstance *createRenderingPipeline(FrameBufferInstance *frameBuffer,
                                                  VkCullModeFlagBits cullMode,
                                                  const char *vertexShader,
                                                  const char *fragmentShader,
                                                  const std::vector<DescriptorInstance *> &descriptorSetsToBind,
                                                  uint32_t pushConstantsSize = 0,
                                                  bool prepareForMesh = false);

        void *createCommandBuffer(PipelineInstance *pipeline) const;
    };
} // Metal

#endif //PIPELINESERVICE_H
