#ifndef STATICGEOMETRYPASS_H
#define STATICGEOMETRYPASS_H

#include "engine/passes/AbstractRenderPass.h"
#include <glm/mat4x4.hpp>

namespace Metal {
    class PipelineService;
    struct WorldRepository;
    class MeshService;
    class VulkanContext;

    struct StaticGeometryPushConstant {
        glm::mat4 model;
        uint32_t renderIndex;
    };

    class StaticGBufferPass final : public AbstractRenderPass {
        StaticGeometryPushConstant pushConstant{};

        PipelineService *pipelineService = nullptr;
        WorldRepository *worldRepository = nullptr;
        MeshService *meshService = nullptr;
        VulkanContext *vulkanContext = nullptr;

        RGResourceHandle globalDataHandle;
        RGResourceHandle materialDataHandle;
        RGResourceHandle primitiveDataHandle;
        RGResourceHandle gBufferHandle;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"PipelineService", &pipelineService},
                {"WorldRepository", &worldRepository},
                {"VulkanContext", &vulkanContext},
                {"MeshService", &meshService}
            };
        }

        void onInitialize() override;

        void setup(RGPassBuilder& builder) override;

        void onSync() override;
    };
} // Metal

#endif // STATICGEOMETRYPASS_H
