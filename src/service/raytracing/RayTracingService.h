#ifndef RAYTRACINGSERVICE_H
#define RAYTRACINGSERVICE_H

#include <memory>
#include <vulkan/vulkan.h>
#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct BufferInstance;

    class RayTracingService final : public AbstractRuntimeComponent {
        // BLAS
        VkAccelerationStructureKHR blas = VK_NULL_HANDLE;
        std::shared_ptr<BufferInstance> blasBuffer = nullptr;

        // TLAS
        VkAccelerationStructureKHR tlas = VK_NULL_HANDLE;
        std::shared_ptr<BufferInstance> tlasBuffer = nullptr;
        std::shared_ptr<BufferInstance> instancesBuffer = nullptr;

        // Scratch buffers (kept alive)
        std::shared_ptr<BufferInstance> blasScratchBuffer = nullptr;
        std::shared_ptr<BufferInstance> tlasScratchBuffer = nullptr;

        bool accelerationStructureBuilt = false;
        bool needsRebuild = true;

        void buildBLAS();

        void buildTLAS();

        void destroyAccelerationStructures();

    public:
        explicit RayTracingService()
            : AbstractRuntimeComponent() {
        }

        void onSync() override;

        [[nodiscard]] bool isReady() const { return accelerationStructureBuilt; }

        void markDirty() { needsRebuild = true; }

        [[nodiscard]] VkAccelerationStructureKHR getTLAS() const { return tlas; }
    };
} // Metal

#endif //RAYTRACINGSERVICE_H
