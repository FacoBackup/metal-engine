#ifndef POSTPROCESSINGPASS_H
#define POSTPROCESSINGPASS_H
#include "../AbstractRenderPass.h"
#include "../../dto/PostProcessingPushConstant.h"

namespace Metal {
    class VulkanContext;
    class PipelineService;
    struct WorldRepository;

    class PostProcessingPass final : public AbstractRenderPass {
        PostProcessingPushConstant pushConstant{};

        VulkanContext *vulkanContext = nullptr;
        PipelineService *pipelineService = nullptr;
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"PipelineService", &pipelineService},
                {"WorldRepository", &worldRepository}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //POSTPROCESSINGPASS_H
