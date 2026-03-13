#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
#include <vector>

#include "../../common/AbstractResourceService.h"
#include "DescriptorSetService.h"
#include "../resource/PipelineInstance.h"

namespace Metal {
    struct PipelineBuilder;
    struct FrameBufferInstance;
    struct PipelineInstance;
    struct DescriptorInstance;

    class VulkanContext;
    class FrameBufferService;
    class BufferService;

    class PipelineService final : public AbstractResourceService<PipelineInstance> {
        VulkanContext &vulkanContext;
        FrameBufferService &framebufferService;
        BufferService &bufferService;
        DescriptorSetService &descriptorSetService;

    public:
        explicit PipelineService(VulkanContext &vulkanContext, FrameBufferService &framebufferService, BufferService &bufferService, DescriptorSetService &descriptorSetService)
            : vulkanContext(vulkanContext), framebufferService(framebufferService), bufferService(bufferService), descriptorSetService(descriptorSetService) {}
        PipelineService() = delete;

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
