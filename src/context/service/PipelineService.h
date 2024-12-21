#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
#include <vector>

#include "../../common/interface/AbstractResourceService.h"
#include "vulkan/vulkan.h"

namespace Metal {
    struct PipelineBuilder;
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

        static void getBlendConfig(bool blendEnabled, VkPipelineColorBlendStateCreateInfo &colorBlending);

        PipelineInstance *createRenderingPipeline(PipelineBuilder &pipelineBuilder);
    };
} // Metal

#endif //PIPELINESERVICE_H
