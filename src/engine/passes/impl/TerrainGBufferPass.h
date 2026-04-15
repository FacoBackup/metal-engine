#ifndef TERRAINGBUFFERPASS_H
#define TERRAINGBUFFERPASS_H

#include "engine/passes/AbstractRenderPass.h"
#include <glm/mat4x4.hpp>

namespace Metal {
    class PipelineService;
    struct TerrainRepository;
    class MeshService;
    class VulkanContext;
    class QuadTreeService;
    class TerrainService;
    class CameraRepository;

    struct TerrainPushConstant {
        glm::mat4 model;
        glm::vec4 region; // x, z, size, lod
        uint32_t renderIndex;
        uint32_t materialIndex;
        uint32_t heightMapIndex;
        float terrainHeight;
        uint32_t layerIndices[4];
    };

    class TerrainGBufferPass final : public AbstractRenderPass {
        TerrainPushConstant pushConstant{};

        PipelineService *pipelineService = nullptr;
        TerrainRepository *terrainRepository = nullptr;
        MeshService *meshService = nullptr;
        VulkanContext *vulkanContext = nullptr;
        QuadTreeService *quadTreeService = nullptr;
        TerrainService *terrainService = nullptr;
        class TextureService *textureService = nullptr;
        CameraRepository *cameraRepository = nullptr;
        class MaterialService *materialService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"PipelineService", &pipelineService},
                {"TerrainRepository", &terrainRepository},
                {"VulkanContext", &vulkanContext},
                {"MeshService", &meshService},
                {"QuadTreeService", &quadTreeService},
                {"TerrainService", &terrainService},
                {"TextureService", &textureService},
                {"CameraRepository", &cameraRepository},
                {"MaterialService", &materialService}
            };
        }

        void onInitialize() override;

        void onSync() override;

        bool shouldRun() override;
    };
} // Metal

#endif // TERRAINGBUFFERPASS_H
