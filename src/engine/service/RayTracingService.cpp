#include "RayTracingService.h"
#include "../../ApplicationContext.h"
#include "../resource/BufferInstance.h"
#include "../dto/DescriptorInstance.h"
#include "../dto/DescriptorBinding.h"
#include "../resource/MeshInstance.h"
#include "../dto/VertexData.h"
#include "../../core/vulkan/VulkanUtils.h"
#include "../../editor/enum/EngineResourceIDs.h"
#include "../../core/vulkan/VulkanContext.h"
#include "../../common/LoggerUtil.h"
#include "../dto/TransformComponent.h"
#include "../dto/PrimitiveComponent.h"
#include "PipelineService.h"
#include "../repository/WorldRepository.h"
#include "MeshService.h"
#include "MaterialService.h"
#include "DescriptorSetService.h"
#include "BufferService.h"
#include "../EngineContext.h"
#include <cstddef>

namespace Metal {
    VkDeviceAddress RayTracingService::getDeviceAddress(VkBuffer buffer) {
        VkBufferDeviceAddressInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = buffer;
        return vulkanContext->vkGetBufferDeviceAddressKHR(vulkanContext->device.device, &info);
    }

    void RayTracingService::updateDescriptorSets(VkAccelerationStructureKHR asHandle) {
        auto descriptors = pipelineService->getAllDescriptors();
        for (auto *descriptor: descriptors) {
            bool needsUpdate = false;
            for (auto &binding: descriptor->bindings) {
                if (binding.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) {
                    binding.accelerationStructure = asHandle;
                    needsUpdate = true;
                }
            }

            if (needsUpdate) {
                descriptorSetService->Write(descriptor->vkDescriptorSet, descriptor->bindings);
            }
        }
    }

    // TODO - EVENT SYSTEM BASED ON WORLD CHANGE AND ENTITY CHANGE
    void RayTracingService::onSync() {
        if (!vulkanContext->rayTracingSupported) {
            return;
        }
        if (needsMaterialUpdate) {
            updateMeshMaterials();
            needsMaterialUpdate = false;
        }
        if (!needsRebuild) {
            return;
        }
        anyMeshes = false;
        // Check if any mesh is present and streamed
        auto view = worldRepository->registry.view<PrimitiveComponent, TransformComponent>();
        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty()) continue;
            auto *instance = meshService->stream(meshComp.meshId);
            if (instance != nullptr && instance->dataBuffer != nullptr && instance->indexBuffer != nullptr) {
                anyMeshes = true;
                break;
            }
        }

        if (!anyMeshes) {
            // No meshes – destroy all structures and set descriptor to null
            dispose(); // destroys BLAS and TLAS (waits for idle)
            updateDescriptorSets(VK_NULL_HANDLE);
            accelerationStructureBuilt = false;
            needsRebuild = false;
            return;
        }

        // At least one mesh exists: rebuild acceleration structures safely
        destroyTLAS(); // Destroy old TLAS (waits for idle) before modifying BLAS
        buildBLAS(); // Rebuild BLAS (incremental, safe because TLAS is gone)
        buildTLAS(); // Build new TLAS

        if (tlas != VK_NULL_HANDLE) {
            LOG_INFO("Updating acceleration structures");
            updateDescriptorSets(tlas);
            accelerationStructureBuilt = true;
        } else {
            LOG_WARN("TLAS build failed or resulted in NULL handle");
            updateDescriptorSets(VK_NULL_HANDLE);
            accelerationStructureBuilt = false;
        }
        needsRebuild = false;
    }

    bool RayTracingService::isReady() const {
        return vulkanContext->rayTracingSupported && accelerationStructureBuilt && anyMeshes && tlas != VK_NULL_HANDLE;
    }

    void RayTracingService::updateMeshMaterials() {
        bool changed = false;
        auto view = worldRepository->registry.view<PrimitiveComponent>();

        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty()) continue;

            if (meshComp.renderIndex < meshMetadata.size()) {
                materialService->load(meshMetadata[meshComp.renderIndex], meshComp);
                changed = true;
            }
        }

        if (changed) {
            for (auto *frame: engineContext->registeredFrames) {
                auto *meshMetadataBuffer = frame->getResourceAs<BufferInstance>(RID_MESH_METADATA_BUFFER);
                if (meshMetadataBuffer != nullptr) {
                    meshMetadataBuffer->update(meshMetadata.data());
                }
            }
        }
    }

    void RayTracingService::destroyTLAS() {
        if (!vulkanContext->rayTracingSupported) {
            return;
        }
        if (vulkanContext->device.device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(vulkanContext->device.device);
        }

        if (tlas != VK_NULL_HANDLE) {
            vulkanContext->vkDestroyAccelerationStructureKHR(vulkanContext->device.device, tlas, nullptr);
            tlas = VK_NULL_HANDLE;
        }

        if (tlasBuffer) {
            bufferService->dispose("tlas_buffer");
            tlasBuffer = nullptr;
        }
        if (instancesBuffer) {
            bufferService->dispose("tlas_instances");
            instancesBuffer = nullptr;
        }
        if (tlasScratchBuffer) {
            bufferService->dispose("tlas_scratch");
            tlasScratchBuffer = nullptr;
        }

        meshMetadata.clear();
    }

    void RayTracingService::buildBLAS() {
        auto &vulkan = vulkanContext;

        std::unordered_map<std::string, MeshInstance *> uniqueMeshes;

        auto view = worldRepository->registry.view<PrimitiveComponent, TransformComponent>();

        for (auto entity: view) {
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty()) continue;
            if (uniqueMeshes.contains(meshComp.meshId)) continue;
            auto *instance = meshService->stream(meshComp.meshId);
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
                if (vulkanContext->device.device != VK_NULL_HANDLE) {
                    vulkanContext->vkDestroyAccelerationStructureKHR(vulkanContext->device.device,
                                                                     existing.accelerationStructure,
                                                                     nullptr);
                }
                if (existing.buffer)
                    bufferService->dispose("blas_" + meshId);
                if (existing.scratchBuffer)
                    bufferService->dispose("blas_scratch_" + meshId);
                blasEntries.erase(meshId);
            }
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

            BLASEntry entry;
            entry.buffer = bufferService->createBuffer(
                "blas_" + meshId,
                sizeInfo.accelerationStructureSize,
                VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                true);

            entry.scratchBuffer = bufferService->createBuffer(
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
                vulkanContext->vkCreateAccelerationStructureKHR(vulkanContext->device.device, &createInfo, nullptr,
                                                                &entry.accelerationStructure));

            buildInfo.dstAccelerationStructure = entry.accelerationStructure;
            buildInfo.scratchData.deviceAddress = getDeviceAddress(entry.scratchBuffer->vkBuffer);

            VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
            rangeInfo.primitiveCount = primitiveCount;
            rangeInfo.primitiveOffset = 0;
            rangeInfo.firstVertex = 0;
            rangeInfo.transformOffset = 0;
            const VkAccelerationStructureBuildRangeInfoKHR *pRangeInfo = &rangeInfo;

            VkCommandBuffer cmd = vulkanContext->beginSingleTimeCommands();
            vulkanContext->vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, &pRangeInfo);
            vulkanContext->endSingleTimeCommands(cmd);

            entry.vertexData = instance->dataBuffer;
            entry.indexData = instance->indexBuffer;
            blasEntries[meshId] = std::move(entry);
        }
    }

    void RayTracingService::buildTLAS() {
        meshMetadata.clear();
        if (blasEntries.empty()) return;

        if (vulkanContext->device.device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(vulkanContext->device.device);
        }

        std::vector<VkAccelerationStructureInstanceKHR> instances;
        auto view = worldRepository->registry.view<PrimitiveComponent, TransformComponent>();

        unsigned int currentInstanceIndex = 0;
        for (auto entity: view) {
            if (currentInstanceIndex >= MAX_MESH_INSTANCES) {
                LOG_ERROR("Max mesh instances reached for ray tracing: " + std::to_string(MAX_MESH_INSTANCES));
                break;
            }
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty()) continue;

            auto it = blasEntries.find(meshComp.meshId);
            if (it == blasEntries.end()) continue;
            LOG_INFO("Building BVH for " + std::to_string(static_cast<unsigned int>(entity)));

            VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
            addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
            addressInfo.accelerationStructure = it->second.accelerationStructure;
            VkDeviceAddress blasAddress = vulkanContext->vkGetAccelerationStructureDeviceAddressKHR(
                vulkanContext->device.device, &addressInfo);

            glm::mat4 model = glm::mat4(1.0f);
            if (worldRepository->registry.all_of<TransformComponent>(entity)) {
                model = worldRepository->registry.get<TransformComponent>(entity).model;
            }

            VkTransformMatrixKHR transform{};
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 4; col++) {
                    transform.matrix[row][col] = model[col][row];
                }
            }

            VkDeviceAddress vertexAddress = getDeviceAddress(it->second.vertexData->vkBuffer);
            VkDeviceAddress indexAddress = getDeviceAddress(it->second.indexData->vkBuffer);

            meshComp.renderIndex = currentInstanceIndex;
            meshMetadata.push_back({meshComp.renderIndex, vertexAddress, indexAddress});

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


        updateMeshMaterials();

        instancesBuffer = bufferService->createBuffer(
            "tlas_instances",
            sizeof(VkAccelerationStructureInstanceKHR) * instances.size(),
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
            instances.data(),
            true);

        VkDeviceAddress instancesAddress = getDeviceAddress(instancesBuffer->vkBuffer);

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
        vulkanContext->vkGetAccelerationStructureBuildSizesKHR(
            vulkanContext->device.device,
            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
            &buildInfo,
            &instanceCount,
            &sizeInfo);

        tlasBuffer = bufferService->createBuffer(
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
            vulkanContext->vkCreateAccelerationStructureKHR(vulkanContext->device.device, &createInfo, nullptr, &tlas));

        tlasScratchBuffer = bufferService->createBuffer(
            "tlas_scratch",
            sizeInfo.buildScratchSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            true);

        buildInfo.dstAccelerationStructure = tlas;
        buildInfo.scratchData.deviceAddress = getDeviceAddress(tlasScratchBuffer->vkBuffer);

        VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
        rangeInfo.primitiveCount = instanceCount;
        const VkAccelerationStructureBuildRangeInfoKHR *pRangeInfo = &rangeInfo;

        VkCommandBuffer cmd = vulkanContext->beginSingleTimeCommands();
        vulkanContext->vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, &pRangeInfo);
        vulkanContext->endSingleTimeCommands(cmd);
    }

    void RayTracingService::dispose() {
        if (!vulkanContext->rayTracingSupported) {
            return;
        }
        if (vulkanContext->device.device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(vulkanContext->device.device);
        }

        // Destroy TLAS and its buffers
        destroyTLAS();

        // Destroy all BLAS entries
        for (auto &[meshId, entry]: blasEntries) {
            if (entry.accelerationStructure != VK_NULL_HANDLE) {
                vulkanContext->vkDestroyAccelerationStructureKHR(vulkanContext->device.device,
                                                                 entry.accelerationStructure, nullptr);
                entry.accelerationStructure = VK_NULL_HANDLE;
            }
            if (entry.buffer)
                bufferService->dispose("blas_" + meshId);
            if (entry.scratchBuffer)
                bufferService->dispose("blas_scratch_" + meshId);
        }
        blasEntries.clear();

        accelerationStructureBuilt = false;
    }
}
