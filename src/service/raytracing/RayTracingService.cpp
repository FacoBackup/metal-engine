#include "RayTracingService.h"
#include "../../context/ApplicationContext.h"
#include "../buffer/BufferInstance.h"
#include "../descriptor/DescriptorInstance.h"
#include "../descriptor/DescriptorBinding.h"
#include "../mesh/MeshInstance.h"
#include "../mesh/VertexData.h"
#include "../../util/VulkanUtils.h"
#include <cstddef>

namespace Metal {
    static VkDeviceAddress getDeviceAddress(const VulkanContext &ctx, VkBuffer buffer) {
        VkBufferDeviceAddressInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = buffer;
        return ctx.vkGetBufferDeviceAddressKHR(ctx.device.device, &info);
    }

    void RayTracingService::onSync() {
        if (!needsRebuild) {
            return;
        }

        bool hasMeshes = false;
        auto view = CTX.worldRepository.registry.view<MeshComponent, TransformComponent>();

        for (auto entity: view) {
            if (CTX.worldRepository.hiddenEntities.contains(static_cast<EntityID>(entity))) continue;
            auto &meshComp = view.get<MeshComponent>(entity);
            if (meshComp.meshId.empty()) continue;
            auto *instance = CTX.streamingRepository.streamMesh(meshComp.meshId);
            if (instance != nullptr && instance->dataBuffer != nullptr && instance->indexBuffer != nullptr) {
                hasMeshes = true;
                break;
            }
        }

        if (!hasMeshes) {
            return;
        }

        destroyAccelerationStructures();
        buildBLAS();
        buildTLAS();

        if (tlas != VK_NULL_HANDLE) {
            LOG_INFO("Updating acceleration structures");
            auto descriptors = CTX.pipelineService.getAllDescriptors();
            for (auto *descriptor: descriptors) {
                bool needsUpdate = false;
                for (auto &binding: descriptor->bindings) {
                    if (binding.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) {
                        binding.accelerationStructure = tlas;
                        needsUpdate = true;
                    }
                }

                if (needsUpdate) {
                    DescriptorInstance::Write(descriptor->vkDescriptorSet, descriptor->bindings);
                }
            }
            accelerationStructureBuilt = true;
            needsRebuild = false;
        }
    }

    void RayTracingService::buildBLAS() {
        auto &vulkan = CTX.vulkanContext;

        std::unordered_map<std::string, MeshInstance *> uniqueMeshes;

        auto view = CTX.worldRepository.registry.view<MeshComponent, TransformComponent>();

        for (auto entity: view) {
            if (CTX.worldRepository.hiddenEntities.contains(static_cast<EntityID>(entity))) continue;
            auto &meshComp = view.get<MeshComponent>(entity);
            if (meshComp.meshId.empty()) continue;
            if (uniqueMeshes.contains(meshComp.meshId)) continue;
            auto *instance = CTX.streamingRepository.streamMesh(meshComp.meshId);
            if (instance == nullptr || instance->dataBuffer == nullptr || instance->indexBuffer == nullptr) {
                continue;
            }
            uniqueMeshes[meshComp.meshId] = instance;
        }

        if (uniqueMeshes.empty()) return;

        for (auto &[meshId, instance]: uniqueMeshes) {
            VkDeviceAddress vertexAddress = getDeviceAddress(vulkan, instance->dataBuffer->vkBuffer);
            VkDeviceAddress indexAddress = getDeviceAddress(vulkan, instance->indexBuffer->vkBuffer);

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
            vulkan.vkGetAccelerationStructureBuildSizesKHR(
                vulkan.device.device,
                VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                &buildInfo,
                &primitiveCount,
                &sizeInfo);

            BLASEntry entry;
            entry.buffer = CTX.bufferService.createBuffer(
                "blas_" + meshId,
                sizeInfo.accelerationStructureSize,
                VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                true);

            VkAccelerationStructureCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
            createInfo.buffer = entry.buffer->vkBuffer;
            createInfo.size = sizeInfo.accelerationStructureSize;
            createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
            VulkanUtils::CheckVKResult(
                vulkan.vkCreateAccelerationStructureKHR(vulkan.device.device, &createInfo, nullptr,
                                                        &entry.accelerationStructure));

            entry.scratchBuffer = CTX.bufferService.createBuffer(
                "blas_scratch_" + meshId,
                sizeInfo.buildScratchSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                true);

            buildInfo.dstAccelerationStructure = entry.accelerationStructure;
            buildInfo.scratchData.deviceAddress = getDeviceAddress(vulkan, entry.scratchBuffer->vkBuffer);

            VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
            rangeInfo.primitiveCount = primitiveCount;
            rangeInfo.primitiveOffset = 0;
            rangeInfo.firstVertex = 0;
            rangeInfo.transformOffset = 0;
            const VkAccelerationStructureBuildRangeInfoKHR *pRangeInfo = &rangeInfo;

            VkCommandBuffer cmd = vulkan.beginSingleTimeCommands();
            vulkan.vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, &pRangeInfo);
            vulkan.endSingleTimeCommands(cmd);

            blasEntries[meshId] = std::move(entry);
        }
    }

    void RayTracingService::buildTLAS() {
        auto &vulkan = CTX.vulkanContext;

        if (blasEntries.empty()) return;

        std::vector<VkAccelerationStructureInstanceKHR> instances;
        auto view = CTX.worldRepository.registry.view<MeshComponent, TransformComponent>();

        for (entt::entity entity: view) {
            if (CTX.worldRepository.hiddenEntities.contains(static_cast<EntityID>(entity))) continue;
            auto &meshComp = view.get<MeshComponent>(entity);
            if (meshComp.meshId.empty()) continue;

            auto it = blasEntries.find(meshComp.meshId);
            if (it == blasEntries.end()) continue;
            LOG_INFO("Building BVH for " + std::to_string(static_cast<unsigned int>(entity)));

            VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
            addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
            addressInfo.accelerationStructure = it->second.accelerationStructure;
            VkDeviceAddress blasAddress = vulkan.vkGetAccelerationStructureDeviceAddressKHR(
                vulkan.device.device, &addressInfo);

            glm::mat4 model = glm::mat4(1.0f);
            if (CTX.worldRepository.registry.all_of<TransformComponent>(entity)) {
                model = CTX.worldRepository.registry.get<TransformComponent>(entity).model;
            }

            VkTransformMatrixKHR transform{};
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 4; col++) {
                    transform.matrix[row][col] = model[col][row];
                }
            }

            VkAccelerationStructureInstanceKHR instance{};
            instance.transform = transform;
            instance.instanceCustomIndex = CTX.materialService.getMaterialIndex(meshComp.materialId);
            instance.mask = 0xFF;
            instance.instanceShaderBindingTableRecordOffset = 0;
            instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
            instance.accelerationStructureReference = blasAddress;

            instances.push_back(instance);
        }

        if (instances.empty()) return;

        instancesBuffer = CTX.bufferService.createBuffer(
            "tlas_instances",
            sizeof(VkAccelerationStructureInstanceKHR) * instances.size(),
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
            instances.data(),
            true);

        VkDeviceAddress instancesAddress = getDeviceAddress(vulkan, instancesBuffer->vkBuffer);

        VkAccelerationStructureGeometryKHR tlasGeometry{};
        tlasGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        tlasGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
        tlasGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        tlasGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
        tlasGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
        tlasGeometry.geometry.instances.data.deviceAddress = instancesAddress;

        VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
        buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        buildInfo.geometryCount = 1;
        buildInfo.pGeometries = &tlasGeometry;

        uint32_t instanceCount = static_cast<uint32_t>(instances.size());

        VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
        vulkan.vkGetAccelerationStructureBuildSizesKHR(
            vulkan.device.device,
            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
            &buildInfo,
            &instanceCount,
            &sizeInfo);

        tlasBuffer = CTX.bufferService.createBuffer(
            "tlas_buffer",
            sizeInfo.accelerationStructureSize,
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            true);

        VkAccelerationStructureCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        createInfo.buffer = tlasBuffer->vkBuffer;
        createInfo.size = sizeInfo.accelerationStructureSize;
        createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        VulkanUtils::CheckVKResult(
            vulkan.vkCreateAccelerationStructureKHR(vulkan.device.device, &createInfo, nullptr, &tlas));

        tlasScratchBuffer = CTX.bufferService.createBuffer(
            "tlas_scratch",
            sizeInfo.buildScratchSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            true);

        buildInfo.dstAccelerationStructure = tlas;
        buildInfo.scratchData.deviceAddress = getDeviceAddress(vulkan, tlasScratchBuffer->vkBuffer);

        VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
        rangeInfo.primitiveCount = instanceCount;
        const VkAccelerationStructureBuildRangeInfoKHR *pRangeInfo = &rangeInfo;

        VkCommandBuffer cmd = vulkan.beginSingleTimeCommands();
        vulkan.vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, &pRangeInfo);
        vulkan.endSingleTimeCommands(cmd);
    }

    void RayTracingService::destroyAccelerationStructures() {
        LOG_INFO("Destroying acceleration structures");
        auto &vulkan = CTX.vulkanContext;

        if (tlas != VK_NULL_HANDLE) {
            vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, tlas, nullptr);
            tlas = VK_NULL_HANDLE;
        }

        for (auto &[meshId, entry]: blasEntries) {
            if (entry.accelerationStructure != VK_NULL_HANDLE) {
                vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, entry.accelerationStructure, nullptr);
            }
            // CTX.bufferService.dispose("blas_" + meshId);
            // CTX.bufferService.dispose("blas_scratch_" + meshId);
        }
        blasEntries.clear();

        // CTX.bufferService.dispose("tlas_buffer");
        // CTX.bufferService.dispose("tlas_instances");
        // CTX.bufferService.dispose("tlas_scratch");
        
        tlasBuffer = nullptr;
        instancesBuffer = nullptr;
        tlasScratchBuffer = nullptr;
        accelerationStructureBuilt = false;
    }
}
