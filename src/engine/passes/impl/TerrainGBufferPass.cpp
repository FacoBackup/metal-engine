#include "TerrainGBufferPass.h"
#include "engine/repository/TerrainRepository.h"
#include "engine/service/PipelineService.h"
#include "engine/service/MeshService.h"
#include "engine/service/QuadTreeService.h"
#include "engine/service/TerrainService.h"
#include "engine/service/TextureService.h"
#include "engine/service/MaterialService.h"
#include "engine/repository/CameraRepository.h"
#include "engine/dto/PipelineBuilder.h"
#include "editor/enum/EngineResourceIDs.h"
#include "core/VulkanContext.h"

#include <algorithm>

namespace Metal {
    void TerrainGBufferPass::onInitialize() {
        PipelineBuilder builder = PipelineBuilder::Of(
                    getScopedResourceId(RID_GBUFFER_RT),
                    "gbuffer/Terrain.vert",
                    "gbuffer/Terrain.frag"
                )
                .enablePrimitiveRendering()
                .setCullMode(VK_CULL_MODE_BACK_BIT)
                .enableDepthTest()
                .setPushConstantsSize(sizeof(TerrainPushConstant))
                .addBufferBinding(getScopedResourceId(RID_GLOBAL_DATA))
                .addBufferBinding(getScopedResourceId(RID_MATERIAL_DATA_BUFFER))
                .addBufferBinding(getScopedResourceId(RID_PRIMITIVE_DATA_BUFFER))
                .addCombinedImageSamplerBinding(vulkanContext->vkImageSampler, VK_NULL_HANDLE,
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        pipelineInstance = pipelineService->createPipeline(builder);
    }

    bool TerrainGBufferPass::shouldRun() {
        return !terrainRepository->heightMap.empty();
    }

    void TerrainGBufferPass::onSync() {
        if (!quadTreeService || !quadTreeService->root) return;

        unsigned int heightMapIndex = textureService->getTextureIndex(terrainRepository->heightMap);
        glm::vec3 cameraPos = cameraRepository->position;
        float terrainHeight = terrainRepository->terrainHeight;

        auto getMatIdx = [&](const std::string &path) {
            if (path.empty()) return 0u;
            StaticGeometryComponent dummy;
            dummy.materialId = path;
            return materialService->getMaterialIndex(dummy);
        };

        auto traverse = [&](auto &self, QuadTreeNode *node) -> void {
            // 1. Frustum Culling
            glm::vec3 min = glm::vec3(node->bounds.x, 0.0f, node->bounds.z);
            glm::vec3 max = glm::vec3(node->bounds.x + node->bounds.size, terrainHeight, node->bounds.z + node->bounds.size);
            if (!cameraRepository->isAABBInsideFrustum(min, max)) {
                return;
            }

            if (node->isLeaf) {
                // 2. Distance-based LOD
                glm::vec3 center = glm::vec3(node->bounds.x + node->bounds.size * 0.5f, 0.0f, node->bounds.z + node->bounds.size * 0.5f);
                float distance = glm::distance(cameraPos, center);

                int lod = static_cast<int>(std::floor(distance / (node->bounds.size * 2.0f)));
                lod = std::clamp(lod, 0, terrainRepository->terrainMaxLOD);

                std::string meshPath = terrainService->getLODMeshPath(lod);
                const auto *meshInstance = meshService->stream(meshPath);
                if (meshInstance) {
                    pushConstant.model = glm::mat4(1.0f);
                    pushConstant.region = glm::vec4(node->bounds.x, node->bounds.z, node->bounds.size, (float)lod);
                    pushConstant.renderIndex = 0;
                    pushConstant.materialIndex = 0;
                    pushConstant.heightMapIndex = heightMapIndex;
                    pushConstant.terrainHeight = terrainHeight;
                    pushConstant.layerIndices[0] = getMatIdx(terrainRepository->layer1);
                    pushConstant.layerIndices[1] = getMatIdx(terrainRepository->layer2);
                    pushConstant.layerIndices[2] = getMatIdx(terrainRepository->layer3);
                    pushConstant.layerIndices[3] = getMatIdx(terrainRepository->layer4);

                    recordPushConstant(&pushConstant);
                    recordDrawMesh(meshInstance);
                }
            } else {
                for (auto &child : node->children) {
                    self(self, child.get());
                }
            }
        };

        traverse(traverse, quadTreeService->root.get());
    }
} // Metal
