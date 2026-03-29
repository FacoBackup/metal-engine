#ifndef DEPTHOFFIELDPASS_H
#define DEPTHOFFIELDPASS_H

#include "../AbstractRenderPass.h"
#include "../../dto/DepthOfFieldPushConstant.h"

namespace Metal {
    class VulkanContext;
    class PipelineService;
    struct EngineRepository;

    class DepthOfFieldPass final : public AbstractRenderPass {
        DepthOfFieldPushConstant pushConstant{};

        VulkanContext *vulkanContext = nullptr;
        PipelineService *pipelineService = nullptr;
        EngineRepository *engineRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"PipelineService", &pipelineService},
                {"EngineRepository", &engineRepository}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
}

#endif //DEPTHOFFIELDPASS_H
