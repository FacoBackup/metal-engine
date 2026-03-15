#ifndef PIPELINESERVICE_H
#define PIPELINESERVICE_H
#include <vector>

#include "../../common/AbstractResourceService.h"
#include "DescriptorSetService.h"
#include "../resource/PipelineInstance.h"

namespace Metal {
    class ShaderService;
    struct PipelineBuilder;
    struct FrameBufferInstance;
    struct PipelineInstance;
    struct DescriptorInstance;

    class VulkanContext;
    class FrameBufferService;
    class BufferService;

    class PipelineService final : public AbstractResourceService<PipelineInstance> {
        VulkanContext *vulkanContext = nullptr;
        FrameBufferService *framebufferService = nullptr;
        BufferService *bufferService = nullptr;
        DescriptorSetService *descriptorSetService = nullptr;
        ShaderService *shaderService = nullptr;

    public:
        PipelineService() = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"FramebufferService", &framebufferService},
                {"BufferService", &bufferService},
                {"DescriptorSetService", &descriptorSetService},
                {"ShaderService", &shaderService}
            };
        }

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
