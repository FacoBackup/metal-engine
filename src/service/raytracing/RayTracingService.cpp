#include "RayTracingService.h"
#include "../../context/ApplicationContext.h"
#include "../buffer/BufferInstance.h"
#include "../descriptor/DescriptorInstance.h"
#include "../descriptor/DescriptorBinding.h"
#include "../mesh/MeshInstance.h"
#include "../mesh/VertexData.h"
#include "../../util/VulkanUtils.h"
#include "../../enum/LevelOfDetail.h"
#include <cstddef>

namespace Metal {
    static VkDeviceAddress getDeviceAddress(const VulkanContext &ctx, VkBuffer buffer) {
        VkBufferDeviceAddressInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = buffer;
        return ctx.vkGetBufferDeviceAddressKHR(ctx.device.device, &info);
    }

    void RayTracingService::updateBVH() {
        // Destroy old TLAS
        auto &vulkan = CTX.vulkanContext;
        if (tlas != VK_NULL_HANDLE) {
            vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, tlas, nullptr);
        }

        // Swap in new results
        blasEntries = std::move(pendingResult->newBlasEntries);
        tlas = pendingResult->newTlas;
        tlasBuffer = std::move(pendingResult->newTlasBuffer);
        instancesBuffer = std::move(pendingResult->newInstancesBuffer);
        tlasScratchBuffer = std::move(pendingResult->newTlasScratchBuffer);

        pendingResult = nullptr;

        // Update descriptor
        if (tlas != VK_NULL_HANDLE) {
            if (CTX.coreDescriptorSets.tlasDescriptor == nullptr) {
                CTX.coreDescriptorSets.tlasDescriptor = std::make_unique<DescriptorInstance>();
                CTX.coreDescriptorSets.tlasDescriptor->addLayoutBinding(
                    DescriptorBinding::OfAccelerationStructure(
                        VK_SHADER_STAGE_RAYGEN_BIT_KHR, 0, tlas));
                CTX.coreDescriptorSets.tlasDescriptor->create();
            } else {
                std::vector<DescriptorBinding> bindings;
                bindings.push_back(
                    DescriptorBinding::OfAccelerationStructure(
                        VK_SHADER_STAGE_RAYGEN_BIT_KHR, 0, tlas));
                DescriptorInstance::Write(CTX.coreDescriptorSets.tlasDescriptor->vkDescriptorSet, bindings);
            }
            accelerationStructureBuilt = true;
        }
    }

    void RayTracingService::checkForChanges(bool &hasChanged) {
        auto view = CTX.worldRepository.registry.view<MeshComponent, TransformComponent>();
        for (auto entity: view) {
            auto &meshComp = CTX.worldRepository.registry.get<MeshComponent>(entity);
            auto &transformComp = CTX.worldRepository.registry.get<TransformComponent>(entity);
            if (meshComp.isNotFrozen() || transformComp.isNotFrozen()) {
                hasChanged = true;
                break;
            }
        }
    }

    void RayTracingService::collectEssentialData(std::vector<MeshBuildData> &meshes,
                                                 std::vector<EntityBuildData> &entities,
                                                 std::unordered_map<std::string, MeshInstance *> &uniqueMeshes,
                                                 MeshComponent &meshComp, TransformComponent &transformComp,
                                                 entt::entity entity) {
        if (CTX.worldRepository.hiddenEntities.contains(static_cast<EntityID>(entity))) return;

        if (meshComp.meshId.empty()) return;

        if (!uniqueMeshes.contains(meshComp.meshId)) {
            auto *instance = CTX.streamingRepository.streamMesh(meshComp.meshId, LevelOfDetail::LOD_0);
            if (instance != nullptr && instance->dataBuffer != nullptr && instance->indexBuffer != nullptr) {
                uniqueMeshes[meshComp.meshId] = instance;
                meshes.push_back({meshComp.meshId, instance});
            }
        }

        EntityBuildData &entityData = entities.emplace_back();

        entityData.meshId = meshComp.meshId;
        entityData.model = transformComp.model;
        entityData.materialIndex = CTX.materialService.getMaterialIndex(meshComp.materialId);
    }

    void RayTracingService::onSync() {
        {
            std::lock_guard<std::mutex> lock(resultMutex);
            if (pendingResult != nullptr) {
                updateBVH();
            }
        }

        if (isBuilding) {
            return;
        }

        bool hasChanged = needsRebuild;

        if (!hasChanged) {
            checkForChanges(hasChanged);
        }

        if (hasChanged) {
            needsRebuild = false;
            isBuilding = true;

            std::vector<MeshBuildData> meshes;
            std::vector<EntityBuildData> entities;

            auto view = CTX.worldRepository.registry.view<MeshComponent, TransformComponent>();
            std::unordered_map<std::string, MeshInstance *> uniqueMeshes;
            for (auto entity: view) {
                auto &meshComp = view.get<MeshComponent>(entity);
                auto &transformComp = view.get<TransformComponent>(entity);
                collectEssentialData(meshes, entities, uniqueMeshes, meshComp, transformComp, entity);
            }

            LOG_INFO("Starting BVH async update");
            // Freeze versions on the render thread
            for (auto entity: view) {
                auto &meshComp = view.get<MeshComponent>(entity);
                auto &transformComp = view.get<TransformComponent>(entity);
                meshComp.freezeVersion();
                transformComp.freezeVersion();
            }

            buildTask = std::async(std::launch::async,
                                   [this, m = std::move(meshes), e = std::move(entities)]() mutable {
                                       asyncBuild(std::move(m), std::move(e));
                                   });
        }
    }

    void RayTracingService::asyncBuild(std::vector<MeshBuildData> meshes, std::vector<EntityBuildData> entities) {
        auto result = std::make_unique<BuildResult>();
        {
            LOG_INFO("Building BLAS");
            std::lock_guard<std::mutex> lock(blasMutex);
            result->newBlasEntries = blasEntries;
            buildBLAS(result->newBlasEntries, meshes);

            LOG_INFO("Building TLAS");
            buildTLAS(result->newBlasEntries, entities, *result);
        }

        {
            std::lock_guard<std::mutex> lock(resultMutex);
            pendingResult = std::move(result);
        }
        isBuilding = false;
    }

    void RayTracingService::buildBLAS(std::unordered_map<std::string, BLASEntry> &currentBlas,
                                      const std::vector<MeshBuildData> &meshes) {
        auto &vulkan = CTX.vulkanContext;

        if (meshes.empty()) return;

        for (const auto &meshData: meshes) {
            const std::string &meshId = meshData.meshId;
            MeshInstance *instance = meshData.instance;
            if (currentBlas.contains(meshId)) continue;

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

            currentBlas[meshId] = std::move(entry);
        }
    }

    void RayTracingService::buildTLAS(const std::unordered_map<std::string, BLASEntry> &currentBlas,
                                      const std::vector<EntityBuildData> &entities,
                                      BuildResult &result) {
        auto &vulkan = CTX.vulkanContext;

        if (currentBlas.empty()) return;

        std::vector<VkAccelerationStructureInstanceKHR> instances;

        for (const auto &entityData: entities) {
            auto it = currentBlas.find(entityData.meshId);
            if (it == currentBlas.end()) continue;

            VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
            addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
            addressInfo.accelerationStructure = it->second.accelerationStructure;
            VkDeviceAddress blasAddress = vulkan.vkGetAccelerationStructureDeviceAddressKHR(
                vulkan.device.device, &addressInfo);

            glm::mat4 model = entityData.model;

            VkTransformMatrixKHR transform{};
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 4; col++) {
                    transform.matrix[row][col] = model[col][row];
                }
            }

            VkAccelerationStructureInstanceKHR instance{};
            instance.transform = transform;
            instance.instanceCustomIndex = entityData.materialIndex;
            instance.mask = 0xFF;
            instance.instanceShaderBindingTableRecordOffset = 0;
            instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
            instance.accelerationStructureReference = blasAddress;

            instances.push_back(instance);
        }

        if (instances.empty()) return;

        result.newInstancesBuffer = CTX.bufferService.createBuffer(
            sizeof(VkAccelerationStructureInstanceKHR) * instances.size(),
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
            instances.data(),
            true);

        VkDeviceAddress instancesAddress = getDeviceAddress(vulkan, result.newInstancesBuffer->vkBuffer);

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

        result.newTlasBuffer = CTX.bufferService.createBuffer(
            sizeInfo.accelerationStructureSize,
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            true);

        VkAccelerationStructureCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        createInfo.buffer = result.newTlasBuffer->vkBuffer;
        createInfo.size = sizeInfo.accelerationStructureSize;
        createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        VulkanUtils::CheckVKResult(
            vulkan.vkCreateAccelerationStructureKHR(vulkan.device.device, &createInfo, nullptr, &result.newTlas));

        result.newTlasScratchBuffer = CTX.bufferService.createBuffer(
            sizeInfo.buildScratchSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            true);

        buildInfo.dstAccelerationStructure = result.newTlas;
        buildInfo.scratchData.deviceAddress = getDeviceAddress(vulkan, result.newTlasScratchBuffer->vkBuffer);

        VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
        rangeInfo.primitiveCount = instanceCount;
        const VkAccelerationStructureBuildRangeInfoKHR *pRangeInfo = &rangeInfo;

        VkCommandBuffer cmd = vulkan.beginSingleTimeCommands();
        vulkan.vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, &pRangeInfo);
        vulkan.endSingleTimeCommands(cmd);
    }

    void RayTracingService::destroyAccelerationStructures() {
        auto &vulkan = CTX.vulkanContext;

        if (tlas != VK_NULL_HANDLE) {
            vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, tlas, nullptr);
            tlas = VK_NULL_HANDLE;
        }

        for (auto &[meshId, entry]: blasEntries) {
            if (entry.accelerationStructure != VK_NULL_HANDLE) {
                vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, entry.accelerationStructure, nullptr);
            }
        }
        blasEntries.clear();

        tlasBuffer = nullptr;
        instancesBuffer = nullptr;
        tlasScratchBuffer = nullptr;
        accelerationStructureBuilt = false;
    }
}
