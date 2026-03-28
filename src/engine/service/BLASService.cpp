#include <engine/service/BLASService.h>
#include <core/VulkanContext.h>
#include <engine/service/BufferService.h>
#include <engine/service/MeshService.h>
#include <engine/resource/MeshInstance.h>
#include <engine/resource/BufferInstance.h>
#include <engine/dto/VertexData.h>
#include <common/VulkanUtils.h>
#include <editor/dto/FieldModificationEvent.h>

#include "engine/dto/ResourceDisposalPayload.h"

namespace Metal {
    VkDeviceAddress BLASService::getDeviceAddress(VkBuffer buffer) const {
        VkBufferDeviceAddressInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = buffer;
        return vulkanContext->vkGetBufferDeviceAddressKHR(vulkanContext->device.device, &info);
    }

    void BLASService::onInitialize() {
        eventListener([this](const Event &event) {
            if (const auto payload = std::dynamic_pointer_cast<ResourceDisposalPayload>(event.payload)) {
                if (const BLASInstance *resource = getResource(payload->resourceId); resource != nullptr) {
                    LOG_INFO("Disposing of mesh instance");
                    dispose(payload->resourceId);
                }
            }
        }, "RESOURCE_DISPOSAL");
    }

    void BLASService::disposeResource(BLASInstance *resource) {
        if (!vulkanContext->rayTracingSupported) return;

        if (vulkanContext->device.device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(vulkanContext->device.device);
        }

        if (resource->accelerationStructure != VK_NULL_HANDLE) {
            vulkanContext->vkDestroyAccelerationStructureKHR(vulkanContext->device.device,
                                                             resource->accelerationStructure, nullptr);
            resource->accelerationStructure = VK_NULL_HANDLE;
        }

        if (resource->buffer) {
            bufferService->dispose(resource->buffer->getId());
            resource->buffer = nullptr;
        }

        if (resource->scratchBuffer) {
            bufferService->dispose(resource->scratchBuffer->getId());
            resource->scratchBuffer = nullptr;
        }
    }

    BLASInstance *BLASService::buildBLAS(const std::string &meshId) {
        if (!vulkanContext->rayTracingSupported) return nullptr;

        auto *instance = meshService->stream(meshId);
        if (instance == nullptr || instance->dataBuffer == nullptr || instance->indexBuffer == nullptr) {
            return nullptr;
        }

        BLASInstance *blas = getResource(meshId);
        if (blas != nullptr) {
            if (blas->vertexData == instance->dataBuffer && blas->indexData == instance->indexBuffer) {
                return blas;
            }
            dispose(meshId);
        }

        blas = createResourceInstance(meshId);

        VkDeviceAddress vertexAddress = getDeviceAddress(instance->dataBuffer->vkBuffer);
        VkDeviceAddress indexAddress = getDeviceAddress(instance->indexBuffer->vkBuffer);

        VkAccelerationStructureGeometryKHR geometry{};
        geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        geometry.geometry.triangles.vertexData.deviceAddress = vertexAddress + offsetof(VertexData, vertex);
        geometry.geometry.triangles.vertexStride = sizeof(VertexData);
        geometry.geometry.triangles.maxVertex = instance->vertexCount - 1;
        geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
        geometry.geometry.triangles.indexData.deviceAddress = indexAddress;
        geometry.geometry.triangles.transformData.deviceAddress = 0;

        uint32_t primitiveCount = instance->indexCount / 3;

        VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
        buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        buildInfo.geometryCount = 1;
        buildInfo.pGeometries = &geometry;

        VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
        vulkanContext->vkGetAccelerationStructureBuildSizesKHR(
            vulkanContext->device.device,
            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
            &buildInfo,
            &primitiveCount,
            &sizeInfo);

        blas->buffer = bufferService->createBuffer(
            "blas_" + meshId,
            sizeInfo.accelerationStructureSize,
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            true);

        blas->scratchBuffer = bufferService->createBuffer(
            "blas_scratch_" + meshId,
            sizeInfo.buildScratchSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            true);

        VkAccelerationStructureCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        createInfo.buffer = blas->buffer->vkBuffer;
        createInfo.size = sizeInfo.accelerationStructureSize;
        createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        VulkanUtils::CheckVKResult(
            vulkanContext->vkCreateAccelerationStructureKHR(vulkanContext->device.device, &createInfo, nullptr,
                                                            &blas->accelerationStructure));

        buildInfo.dstAccelerationStructure = blas->accelerationStructure;
        buildInfo.scratchData.deviceAddress = getDeviceAddress(blas->scratchBuffer->vkBuffer);

        VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
        rangeInfo.primitiveCount = primitiveCount;
        rangeInfo.primitiveOffset = 0;
        rangeInfo.firstVertex = 0;
        rangeInfo.transformOffset = 0;
        const VkAccelerationStructureBuildRangeInfoKHR *pRangeInfo = &rangeInfo;

        VkCommandBuffer cmd = vulkanContext->beginSingleTimeCommands();
        vulkanContext->vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, &pRangeInfo);
        vulkanContext->endSingleTimeCommands(cmd);

        blas->vertexData = instance->dataBuffer;
        blas->indexData = instance->indexBuffer;

        return blas;
    }

    VkDeviceAddress BLASService::getBLASAddress(const std::string &meshId) {
        BLASInstance *blas = buildBLAS(meshId);
        if (blas == nullptr) return 0;

        VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
        addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        addressInfo.accelerationStructure = blas->accelerationStructure;
        return vulkanContext->vkGetAccelerationStructureDeviceAddressKHR(vulkanContext->device.device, &addressInfo);
    }
}
