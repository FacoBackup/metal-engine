#ifndef SELECTIONIDPASS_H
#define SELECTIONIDPASS_H

#include "../../engine/passes/AbstractRenderPass.h"
#include "../dto/SelectedDotPushConstant.h"

namespace Metal {
    class PipelineService;
    struct EditorRepository;
    struct WorldRepository;
    class MeshService;

    class SelectionIDPass final : public AbstractRenderPass {
        SelectedDotPushConstant pushConstant{};

        PipelineService *pipelineService = nullptr;
        EditorRepository *editorRepository = nullptr;
        WorldRepository *worldRepository = nullptr;
        MeshService *meshService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"PipelineService", pipelineService},
                {"EditorRepository", editorRepository},
                {"WorldRepository", worldRepository},
                {"MeshService", meshService}
            };
        }

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif // SELECTIONIDPASS_H
