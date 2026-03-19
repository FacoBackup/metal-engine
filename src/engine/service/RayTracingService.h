#ifndef RAYTRACINGSERVICE_H
#define RAYTRACINGSERVICE_H

#include "../dto/MeshMetadata.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>
#include "../../common/IService.h"
#include "../../common/ISync.h"
#include "../../common/IDisposable.h"
#include "../../common/IInit.h"
#include "../../editor/service/EventService.h"

namespace Metal {
    class DescriptorSetService;
    class BufferInstance;
    class VulkanContext;
    class PipelineService;
    struct WorldRepository;
    class MeshService;
    class MaterialService;
    class BufferService;
    class EngineContext;

    class EventService;

    class RayTracingService final : public IService, public ISync, public IDisposable, public IInit {
        VulkanContext *vulkanContext = nullptr;
        PipelineService *pipelineService = nullptr;
        WorldRepository *worldRepository = nullptr;
        MeshService *meshService = nullptr;
        MaterialService *materialService = nullptr;
        BufferService *bufferService = nullptr;
        EngineContext *engineContext = nullptr;
        DescriptorSetService *descriptorSetService = nullptr;
        EventService *eventService = nullptr;

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

        VkDeviceAddress getDeviceAddress(VkBuffer buffer) const;

        void updateDescriptorSets(VkAccelerationStructureKHR asHandle) const;

        void buildBLAS();

        void buildTLAS();

        void destroyTLAS();

        void updateMeshMaterials();

    public:
        RayTracingService() = default;

        std::vector<Dependency> getDependencies() override {
            return {
                {"VulkanContext", &vulkanContext},
                {"PipelineService", &pipelineService},
                {"WorldRepository", &worldRepository},
                {"MeshService", &meshService},
                {"MaterialService", &materialService},
                {"BufferService", &bufferService},
                {"EngineContext", &engineContext},
                {"DescriptorSetService", &descriptorSetService},
                {"EventService", &eventService}
            };
        }

        void dispose() override;

        void onSync() override;

        void onInitialize() override;

        [[nodiscard]] bool isReady() const;

        void markDirty() { needsRebuild = true; }

        [[nodiscard]] VkAccelerationStructureKHR getTLAS() const { return tlas; }
    };
} // Metal

#endif //RAYTRACINGSERVICE_H
