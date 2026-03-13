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
    struct BufferInstance;

    class RayTracingService final : public AbstractRuntimeComponent {
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
