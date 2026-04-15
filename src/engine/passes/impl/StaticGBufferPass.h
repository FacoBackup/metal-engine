#ifndef STATICGEOMETRYPASS_H
#define STATICGEOMETRYPASS_H

#include "engine/passes/AbstractRenderPass.h"
#include <glm/mat4x4.hpp>

namespace Metal {
    class PipelineService;
    struct WorldRepository;
    class MeshService;
    class VulkanContext;
    class QuadTreeService;

    struct StaticGeometryPushConstant {
        glm::mat4 model;
        glm::mat4 previousModel;
        uint32_t renderIndex;
        uint32_t materialIndex;
        float parallaxHeightScale;
        int32_t parallaxLayers;
    };

    class StaticGBufferPass final : public AbstractRenderPass {
        StaticGeometryPushConstant pushConstant{};

        PipelineService *pipelineService = nullptr;
        WorldRepository *worldRepository = nullptr;
        MeshService *meshService = nullptr;
        VulkanContext *vulkanContext = nullptr;
        QuadTreeService *quadTreeService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"PipelineService", &pipelineService},
                {"WorldRepository", &worldRepository},
                {"VulkanContext", &vulkanContext},
                {"MeshService", &meshService},
                {"QuadTreeService", &quadTreeService}
            };
        }

        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif // STATICGEOMETRYPASS_H
