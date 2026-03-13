#ifndef RAYTRACINGSERVICE_H
#define RAYTRACINGSERVICE_H

#include "../dto/MeshMetadata.h"
#include "../../editor/enum/engine-definitions.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>
#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    class BufferInstance;
    class VulkanContext;
    class PipelineService;
    struct WorldRepository;
    class MeshService;
    class MaterialService;
    class BufferService;
    class EngineContext;

    class RayTracingService final : public AbstractRuntimeComponent {
        VulkanContext &vulkanContext;
        PipelineService &pipelineService;
        WorldRepository &worldRepository;
        MeshService &meshService;
        MaterialService &materialService;
        BufferService &bufferService;
        EngineContext &engineContext;

        struct BLASEntry {
            VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;
            BufferInstance *buffer = nullptr;
            BufferInstance *scratchBuffer = nullptr;
            BufferInstance *vertexData = nullptr;
            BufferInstance *indexData = nullptr;
        };
        bool anyMeshes = false;

        // One BLAS per unique mesh ID
        std::unordered_map<std::string, BLASEntry> blasEntries;

        // TLAS
        VkAccelerationStructureKHR tlas = VK_NULL_HANDLE;
        BufferInstance *tlasBuffer = nullptr;
        BufferInstance *instancesBuffer = nullptr;

        // Scratch buffer for TLAS
        BufferInstance *tlasScratchBuffer = nullptr;

        std::vector<MeshMetadata> meshMetadata{};

        bool accelerationStructureBuilt = false;
        bool needsRebuild = true;
        bool needsMaterialUpdate = false;

        void updateDescriptorSets(VkAccelerationStructureKHR asHandle);

        void buildBLAS();

        void buildTLAS();

        void destroyTLAS();

        void updateMeshMaterials();

    public:
        RayTracingService(VulkanContext &vulkanContext, PipelineService &pipelineService, WorldRepository &worldRepository, MeshService &meshService, MaterialService &materialService, BufferService &bufferService, EngineContext &engineContext)
            : vulkanContext(vulkanContext), pipelineService(pipelineService), worldRepository(worldRepository), meshService(meshService), materialService(materialService), bufferService(bufferService), engineContext(engineContext) {}
        RayTracingService() = delete;

        void setNeedsMaterialUpdate(bool val) {
            needsMaterialUpdate = val;
        }

        void destroyAccelerationStructures();

        void onSync() override;

        [[nodiscard]] bool isReady() const;

        void markDirty() { needsRebuild = true; }

        [[nodiscard]] VkAccelerationStructureKHR getTLAS() const { return tlas; }
    };
} // Metal

#endif //RAYTRACINGSERVICE_H
