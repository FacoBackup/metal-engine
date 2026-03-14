#ifndef GRIDRENDERPASS_H
#define GRIDRENDERPASS_H
#include "../../engine/passes/AbstractRenderPass.h"
#include "../dto/GridPushConstant.h"

namespace Metal {
    class PipelineService;
    class ApplicationContext;
    struct EditorRepository;

    class GridPass final : public AbstractRenderPass {
        GridPushConstant pushConstant{};

        PipelineService *pipelineService = nullptr;
        ApplicationContext *applicationContext = nullptr;
        EditorRepository *editorRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"PipelineService", pipelineService},
                {"ApplicationContext", applicationContext},
                {"EditorRepository", editorRepository}
            };
        }

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif //GRIDRENDERPASS_H
