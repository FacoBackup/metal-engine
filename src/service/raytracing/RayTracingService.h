#ifndef RAYTRACINGSERVICE_H
#define RAYTRACINGSERVICE_H

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
            std::shared_ptr<BufferInstance> buffer = nullptr;
            std::shared_ptr<BufferInstance> scratchBuffer = nullptr;
        };

        // One BLAS per unique mesh ID
        std::unordered_map<std::string, BLASEntry> blasEntries;

        // TLAS
        VkAccelerationStructureKHR tlas = VK_NULL_HANDLE;
        std::shared_ptr<BufferInstance> tlasBuffer = nullptr;
        std::shared_ptr<BufferInstance> instancesBuffer = nullptr;

        // Scratch buffer for TLAS
        std::shared_ptr<BufferInstance> tlasScratchBuffer = nullptr;

        bool accelerationStructureBuilt = false;
        bool needsRebuild = true;

        void buildBLAS();

        void buildTLAS();


    public:
        void destroyAccelerationStructures();

        void onSync() override;

        [[nodiscard]] bool isReady() const { return accelerationStructureBuilt; }

        void markDirty() { needsRebuild = true; }

        [[nodiscard]] VkAccelerationStructureKHR getTLAS() const { return tlas; }
    };
} // Metal

#endif //RAYTRACINGSERVICE_H
