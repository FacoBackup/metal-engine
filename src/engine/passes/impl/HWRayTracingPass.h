#ifndef HWRAYTRACINGPASS_H
#define HWRAYTRACINGPASS_H
#include "../AbstractComputePass.h"
#include "../../dto/HWRayTracingPushConstant.h"

namespace Metal {
    class RayTracingService;
    class PipelineService;
    class VulkanContext;
    class EngineContext;
    struct EngineRepository;

    class HWRayTracingPass final : public AbstractComputePass {
        bool isFirstRun = true;
        HWRayTracingPushConstant pushConstant{};

        RayTracingService *rayTracingService = nullptr;
        PipelineService *pipelineService = nullptr;
        VulkanContext *vulkanContext = nullptr;
        EngineContext *engineContext = nullptr;
        EngineRepository *engineRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"RayTracingService", rayTracingService},
                {"PipelineService", pipelineService},
                {"VulkanContext", vulkanContext},
                {"EngineContext", engineContext},
                {"EngineRepository", engineRepository}
            };
        }

        void onSync() override;

        void onInitialize() override;
    };
} // Metal

#endif //HWRAYTRACINGPASS_H
