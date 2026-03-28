#ifndef DEBUGPHONGPASS_H
#define DEBUGPHONGPASS_H

#include "engine/passes/AbstractRenderPass.h"
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace Metal {
    class PipelineService;
    struct EditorRepository;
    struct WorldRepository;
    class MeshService;

    struct DebugPhongPushConstant {
        glm::mat4 model;
        uint32_t renderIndex;
    };

    class FallbackPass final : public AbstractRenderPass {
        DebugPhongPushConstant pushConstant{};

        PipelineService *pipelineService = nullptr;
        EditorRepository *editorRepository = nullptr;
        WorldRepository *worldRepository = nullptr;
        MeshService *meshService = nullptr;
        VulkanContext *vulkanContext = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"PipelineService", &pipelineService},
                {"EditorRepository", &editorRepository},
                {"WorldRepository", &worldRepository},
                {"VulkanContext", &vulkanContext},
                {"MeshService", &meshService}
            };
        }

        void onInitialize() override;

        bool shouldRun() override;

        void onSync() override;
    };
} // Metal

#endif // DEBUGPHONGPASS_H
