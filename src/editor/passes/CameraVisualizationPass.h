#ifndef CAMERAVISUALIZATIONPASS_H
#define CAMERAVISUALIZATIONPASS_H

#include "engine/passes/AbstractRenderPass.h"
#include "../dto/CameraVisualizationPushConstant.h"

namespace Metal {
    class PipelineService;
    struct EditorRepository;
    struct WorldRepository;

    class CameraVisualizationPass final : public AbstractRenderPass {
        CameraVisualizationPushConstant pushConstant{};

        PipelineService *pipelineService = nullptr;
        EditorRepository *editorRepository = nullptr;
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"PipelineService", &pipelineService},
                {"EditorRepository", &editorRepository},
                {"WorldRepository", &worldRepository}
            };
        }

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif // CAMERAVISUALIZATIONPASS_H
