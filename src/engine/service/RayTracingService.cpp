#include <engine/service/RayTracingService.h>
#include <core/VulkanContext.h>
#include <engine/service/BLASService.h>
#include <engine/service/TLASService.h>
#include <engine/service/PrimitiveService.h>
#include <engine/repository/WorldRepository.h>
#include <engine/dto/PrimitiveComponent.h>
#include <engine/dto/TransformComponent.h>
#include <engine/service/MeshService.h>
#include <engine/resource/MeshInstance.h>
#include <common/LoggerUtil.h>

namespace Metal {
    void RayTracingService::onInitialize() {
    }

    void RayTracingService::onSync() {
        if (!vulkanContext->rayTracingSupported) {
            return;
        }

        blasService->onSync();
        tlasService->onSync();
        primitiveService->onSync();

        if (tlasService->getTLAS() != VK_NULL_HANDLE) {
            accelerationStructureBuilt = true;
        } else {
            accelerationStructureBuilt = false;
        }
    }

    bool RayTracingService::isReady() const {
        return vulkanContext->rayTracingSupported && accelerationStructureBuilt && tlasService->getTLAS() != VK_NULL_HANDLE;
    }

    void RayTracingService::dispose() {
        if (!vulkanContext->rayTracingSupported) {
            return;
        }
        tlasService->dispose();
        accelerationStructureBuilt = false;
    }

    VkAccelerationStructureKHR RayTracingService::getTLAS() const {
        return tlasService->getTLAS();
    }
}
