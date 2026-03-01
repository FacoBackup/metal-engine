#include "RayTracingService.h"
#include "../../context/ApplicationContext.h"
#include "../buffer/BufferInstance.h"
#include "../descriptor/DescriptorInstance.h"
#include "../descriptor/DescriptorBinding.h"
#include "../mesh/MeshInstance.h"
#include "../mesh/VertexData.h"
#include "../../util/VulkanUtils.h"
#include "../../enum/EngineResourceIDs.h"
#include <cstddef>

namespace Metal {
    static VkDeviceAddress getDeviceAddress(const VulkanContext &ctx, VkBuffer buffer) {
        VkBufferDeviceAddressInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = buffer;
        return ctx.vkGetBufferDeviceAddressKHR(ctx.device.device, &info);
    }

    void RayTracingService::updateDescriptorSets(VkAccelerationStructureKHR asHandle) {
        auto descriptors = CTX.pipelineService.getAllDescriptors();
        for (auto *descriptor: descriptors) {
            bool needsUpdate = false;
            for (auto &binding: descriptor->bindings) {
                if (binding.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) {
                    binding.accelerationStructure = asHandle;
                    needsUpdate = true;
                }
            }

            if (needsUpdate) {
                DescriptorSetService::Write(descriptor->vkDescriptorSet, descriptor->bindings);
            }
        }
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
            auto *instance = CTX.streamingService.streamMesh(meshComp.meshId);
            if (instance != nullptr && instance->dataBuffer != nullptr && instance->indexBuffer != nullptr) {
                hasMeshes = true;
                break;
            }
        }

        if (!hasMeshes) {
            destroyAccelerationStructures();
            updateDescriptorSets(VK_NULL_HANDLE);
            needsRebuild = false;
            return;
        }

        buildBLAS();
        buildTLAS();

        if (tlas != VK_NULL_HANDLE) {
            LOG_INFO("Updating acceleration structures");
            updateDescriptorSets(tlas);
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
            auto *instance = CTX.streamingService.streamMesh(meshComp.meshId);
            if (instance == nullptr || instance->dataBuffer == nullptr || instance->indexBuffer == nullptr) {
                continue;
            }
            uniqueMeshes[meshComp.meshId] = instance;
        }

        if (uniqueMeshes.empty()) return;

        for (auto &[meshId, instance]: uniqueMeshes) {
            if (blasEntries.contains(meshId)) {
                auto &existing = blasEntries[meshId];
                if (existing.vertexData == instance->dataBuffer && existing.indexData == instance->indexBuffer) {
                    continue;
                }
                // If mesh buffers changed, we need to rebuild this BLAS
                // For now, let's just destroy and re-create below by removing it
                if (vulkan.device.device != VK_NULL_HANDLE) {
                    vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, existing.accelerationStructure, nullptr);
                }
                if (existing.buffer) CTX.bufferService.dispose("blas_" + meshId);
                if (existing.scratchBuffer) CTX.bufferService.dispose("blas_scratch_" + meshId);
                blasEntries.erase(meshId);
            }
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

            entry.scratchBuffer = CTX.bufferService.createBuffer(
                "blas_scratch_" + meshId,
                sizeInfo.buildScratchSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
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

            entry.vertexData = instance->dataBuffer;
            entry.indexData = instance->indexBuffer;
            blasEntries[meshId] = std::move(entry);
        }
    }

    void RayTracingService::buildTLAS() {
        auto &vulkan = CTX.vulkanContext;
        if (blasEntries.empty()) return;

        if (vulkan.device.device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(vulkan.device.device);
        }

        if (tlas != VK_NULL_HANDLE) {
            vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, tlas, nullptr);
            tlas = VK_NULL_HANDLE;
        }

        if (tlasBuffer) CTX.bufferService.dispose("tlas_buffer");
        if (instancesBuffer) CTX.bufferService.dispose("tlas_instances");
        if (tlasScratchBuffer) CTX.bufferService.dispose("tlas_scratch");

        tlasBuffer = nullptr;
        instancesBuffer = nullptr;
        tlasScratchBuffer = nullptr;

        std::vector<VkAccelerationStructureInstanceKHR> instances;
        auto view = CTX.worldRepository.registry.view<MeshComponent, TransformComponent>();

        unsigned int currentInstanceIndex = 0;
        for (auto entity: view) {
            if (currentInstanceIndex >= MAX_MESH_INSTANCES) {
                LOG_ERROR("Max mesh instances reached for ray tracing: " + std::to_string(MAX_MESH_INSTANCES));
                break;
            }
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

            uint32_t materialIndex = CTX.materialService.getMaterialIndex(meshComp.materialId);

            VkDeviceAddress vertexAddress = getDeviceAddress(vulkan, it->second.vertexData->vkBuffer);
            VkDeviceAddress indexAddress = getDeviceAddress(vulkan, it->second.indexData->vkBuffer);

            meshComp.renderIndex = currentInstanceIndex;
            meshMetadata.push_back({meshComp.renderIndex, materialIndex, vertexAddress, indexAddress});

            VkAccelerationStructureInstanceKHR instance{};
            instance.transform = transform;
            instance.instanceCustomIndex = currentInstanceIndex;
            instance.mask = 0xFF;
            instance.instanceShaderBindingTableRecordOffset = 0;
            instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
            instance.accelerationStructureReference = blasAddress;

            instances.push_back(instance);
            currentInstanceIndex++;
        }

        if (instances.empty()) return;

        if (CTX.engineContext.currentFrame != nullptr) {
            auto *meshMetadataBuffer = CTX.engineContext.currentFrame->getResourceAs<BufferInstance>(RID_MESH_METADATA_BUFFER);
            if (meshMetadataBuffer != nullptr) {
                meshMetadataBuffer->update(meshMetadata.data());
            }
        }

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

        if (vulkan.device.device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(vulkan.device.device);
        }

        if (tlas != VK_NULL_HANDLE) {
            vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, tlas, nullptr);
            tlas = VK_NULL_HANDLE;
        }

        for (auto &[meshId, entry]: blasEntries) {
            if (entry.accelerationStructure != VK_NULL_HANDLE) {
                vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, entry.accelerationStructure, nullptr);
                entry.accelerationStructure = VK_NULL_HANDLE;
            }
            if (entry.buffer) CTX.bufferService.dispose("blas_" + meshId);
            if (entry.scratchBuffer) CTX.bufferService.dispose("blas_scratch_" + meshId);
        }
        blasEntries.clear();

        if (tlasBuffer) CTX.bufferService.dispose("tlas_buffer");
        if (instancesBuffer) CTX.bufferService.dispose("tlas_instances");
        if (tlasScratchBuffer) CTX.bufferService.dispose("tlas_scratch");

        tlasBuffer = nullptr;
        instancesBuffer = nullptr;
        tlasScratchBuffer = nullptr;
        accelerationStructureBuilt = false;
    }
}
