#include "CorePipelines.h"

#include "MeshPushConstant.h"
#include "../ApplicationContext.h"
#include "../../common/util/VulkanUtils.h"
#include "../runtime/MeshData.h"
#include "../runtime/PipelineInstance.h"
#include "../service//PipelineService.h"

namespace Metal {
    static MeshData createCube() {
        MeshData cube{"cube", {}, {}};

        // Define the 8 unique vertices of the cube
        glm::vec3 vertices[] = {
            {-1.0f, -1.0f, -1.0f}, // 0
            { 1.0f, -1.0f, -1.0f}, // 1
            { 1.0f,  1.0f, -1.0f}, // 2
            {-1.0f,  1.0f, -1.0f}, // 3
            {-1.0f, -1.0f,  1.0f}, // 4
            { 1.0f, -1.0f,  1.0f}, // 5
            { 1.0f,  1.0f,  1.0f}, // 6
            {-1.0f,  1.0f,  1.0f}  // 7
        };

        // Define the normals for each face of the cube
        glm::vec3 normals[] = {
            { 0.0f,  0.0f, -1.0f}, // Back
            { 0.0f,  0.0f,  1.0f}, // Front
            { 0.0f, -1.0f,  0.0f}, // Bottom
            { 0.0f,  1.0f,  0.0f}, // Top
            {-1.0f,  0.0f,  0.0f}, // Left
            { 1.0f,  0.0f,  0.0f}  // Right
        };

        // Define the UV coordinates
        glm::vec2 uvs[] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f}
        };

        // Define the vertex data for each face of the cube
        uint32_t faceIndices[] = {
            0, 1, 2, 2, 3, 0, // Back
            4, 5, 6, 6, 7, 4, // Front
            0, 4, 5, 5, 1, 0, // Bottom
            3, 7, 6, 6, 2, 3, // Top
            0, 3, 7, 7, 4, 0, // Left
            1, 5, 6, 6, 2, 1  // Right
        };

        for (int face = 0; face < 6; ++face) {
            for (int i = 0; i < 6; ++i) {
                uint32_t index = faceIndices[face * 6 + i];
                cube.data.push_back({vertices[index], normals[face], uvs[i % 4]});
                cube.indices.push_back(static_cast<uint32_t>(cube.data.size() - 1));
            }
        }

        return cube;
    }

    void CorePipelines::onInitialize() {
        debugPipeline = pipelineService.createRenderingPipeline(
            context.getVulkanContext().coreFrameBuffers.auxRenderPass,
            VK_CULL_MODE_NONE,
            "../resources/shaders/DEBUG.vert",
            "../resources/shaders/DEBUG.frag",
            {context.getVulkanContext().coreDescriptorSets.globalDataDescriptor},
            sizeof(MeshPushConstant),
            true);

        gridPipeline = pipelineService.createRenderingPipeline(
            context.getVulkanContext().coreFrameBuffers.auxRenderPass,
            VK_CULL_MODE_NONE,
            "../resources/shaders/QUAD.vert",
            "../resources/shaders/tool/GRID.frag");

        sampleMesh = context.getVulkanContext().meshService.createMesh(createCube());
    }

    void CorePipelines::dispose() {
        // TODO - Add button to UI to re-create pipelines and reload shaders
        // TODO - Discard shader module after pipeline is built
        debugPipeline->dispose(vulkanContext);
    }
} // Metal
