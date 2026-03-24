#ifndef RAYTRACINGSERVICE_H
#define RAYTRACINGSERVICE_H

#include <engine/dto/MeshMetadata.h>
#include <vulkan/vulkan.h>
#include <common/IService.h>
#include <common/ISync.h>
#include <common/IDisposable.h>
#include <common/IInit.h>
#include <common/IEventMember.h>
#include <ApplicationEventContext.h>

namespace Metal {
    class DescriptorSetService;
    struct BufferInstance;
    class VulkanContext;
    class PipelineService;
    struct WorldRepository;
    class MeshService;
    class MaterialService;
    class BufferService;
    class EngineContext;
    class BLASService;
    class TLASService;
    class PrimitiveService;

    class ApplicationEventContext;

    class RayTracingService final : public IService, public IEventMember, public ISync, public IDisposable,
                                    public IInit {
        VulkanContext *vulkanContext = nullptr;
        PipelineService *pipelineService = nullptr;
        WorldRepository *worldRepository = nullptr;
        MeshService *meshService = nullptr;
        MaterialService *materialService = nullptr;
        BufferService *bufferService = nullptr;
        EngineContext *engineContext = nullptr;
        DescriptorSetService *descriptorSetService = nullptr;
        BLASService *blasService = nullptr;
        TLASService *tlasService = nullptr;
        PrimitiveService *primitiveService = nullptr;

        bool accelerationStructureBuilt = false;

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
                {"BLASService", &blasService},
                {"TLASService", &tlasService},
                {"PrimitiveService", &primitiveService}
            };
        }

        void dispose() override;

        void onSync() override;

        void onInitialize() override;

        [[nodiscard]] bool isReady() const;

        [[nodiscard]] VkAccelerationStructureKHR getTLAS() const;
    };
} // Metal

#endif //RAYTRACINGSERVICE_H
