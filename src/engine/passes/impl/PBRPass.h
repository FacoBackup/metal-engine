#ifndef DEPTHOFFIELDPASS_H
#define DEPTHOFFIELDPASS_H

#include "../AbstractRenderPass.h"

namespace Metal {
    struct EngineRepository;
    class VulkanContext;
    class PipelineService;
    struct WorldRepository;

    class PBRPass final : public AbstractRenderPass {
        EngineRepository *engineRepository = nullptr;
        VulkanContext *vulkanContext = nullptr;
        PipelineService *pipelineService = nullptr;
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"PipelineService", &pipelineService},
                {"EngineRepository", &engineRepository},
                {"WorldRepository", &worldRepository}
            };
        }

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
}

#endif //DEPTHOFFIELDPASS_H
