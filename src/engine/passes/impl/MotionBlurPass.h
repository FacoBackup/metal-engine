#ifndef MOTIONBLURPASS_H
#define MOTIONBLURPASS_H

#include "../AbstractRenderPass.h"
#include "../../dto/MotionBlurPushConstant.h"

namespace Metal {
    class VulkanContext;
    class PipelineService;
    struct CameraRepository;

    class MotionBlurPass final : public AbstractRenderPass {
        MotionBlurPushConstant pushConstant{};

        VulkanContext *vulkanContext = nullptr;
        PipelineService *pipelineService = nullptr;
        CameraRepository *cameraRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"PipelineService", &pipelineService},
                {"CameraRepository", &cameraRepository}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
}

#endif //MOTIONBLURPASS_H
