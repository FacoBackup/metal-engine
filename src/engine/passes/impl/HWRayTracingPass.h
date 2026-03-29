#ifndef HWRAYTRACINGPASS_H
#define HWRAYTRACINGPASS_H
#include "../AbstractComputePass.h"
#include "../../dto/HWRayTracingPushConstant.h"

namespace Metal {
    class TLASService;
    class PipelineService;
    class VulkanContext;
    class EngineContext;
    class DescriptorSetService;
    struct EngineRepository;
    class ApplicationEventContext;

    class HWRayTracingPass final : public AbstractComputePass {
        bool isFirstRun = true;
        bool needsUpdate = true;
        HWRayTracingPushConstant pushConstant{};

        TLASService *tlasService = nullptr;
        PipelineService *pipelineService = nullptr;
        VulkanContext *vulkanContext = nullptr;
        EngineContext *engineContext = nullptr;
        DescriptorSetService *descriptorSetService = nullptr;
        EngineRepository *engineRepository = nullptr;
        uint32_t frameIndex = 0;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"TLASService", &tlasService},
                {"PipelineService", &pipelineService},
                {"VulkanContext", &vulkanContext},
                {"EngineContext", &engineContext},
                {"DescriptorSetService", &descriptorSetService},
                {"EngineRepository", &engineRepository}
            };
        }

        void onSync() override;

        void onInitialize() override;

        bool requiresRayTracing() override {
            return true;
        }
    };
} // Metal

#endif //HWRAYTRACINGPASS_H
