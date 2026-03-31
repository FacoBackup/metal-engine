#ifndef POSTPROCESSINGPASS_H
#define POSTPROCESSINGPASS_H
#include "../AbstractRenderPass.h"
#include "../../dto/PostProcessingPushConstant.h"

namespace Metal {
    struct CameraRepository;
    class VulkanContext;
    class PipelineService;

    class PostProcessingPass final : public AbstractRenderPass {
        PostProcessingPushConstant pushConstant{};

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
} // Metal

#endif //POSTPROCESSINGPASS_H
