#ifndef MOTIONBLURPASS_H
#define MOTIONBLURPASS_H

#include "../AbstractRenderPass.h"
#include "../../dto/MotionBlurPushConstant.h"

namespace Metal {
    class VulkanContext;
    class PipelineService;
    struct WorldRepository;

    class MotionBlurPass final : public AbstractRenderPass {
        MotionBlurPushConstant pushConstant{};

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
}

#endif //MOTIONBLURPASS_H
