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
    class HistoryEventService;

    class HWRayTracingPass final : public AbstractComputePass {
        bool isFirstRun = true;
        bool needsUpdate = true;
        HWRayTracingPushConstant pushConstant{};

        RayTracingService *rayTracingService = nullptr;
        PipelineService *pipelineService = nullptr;
        VulkanContext *vulkanContext = nullptr;
        EngineContext *engineContext = nullptr;
        EngineRepository *engineRepository = nullptr;
        HistoryEventService *eventService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"RayTracingService", &rayTracingService},
                {"PipelineService", &pipelineService},
                {"VulkanContext", &vulkanContext},
                {"EngineContext", &engineContext},
                {"HistoryEventService", &eventService},
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
