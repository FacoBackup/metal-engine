#include "RayTracingService.h"
#include "../../context/ApplicationContext.h"
#include "../buffer/BufferInstance.h"
#include "../descriptor/DescriptorInstance.h"
#include "../descriptor/DescriptorBinding.h"
#include "../mesh/MeshInstance.h"
#include "../mesh/VertexData.h"
#include "../../util/VulkanUtils.h"
#include "../../enum/LevelOfDetail.h"

namespace Metal {
    static VkDeviceAddress getDeviceAddress(const VulkanContext &ctx, VkBuffer buffer) {
        VkBufferDeviceAddressInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = buffer;
        return ctx.vkGetBufferDeviceAddressKHR(ctx.device.device, &info);
    }


    void RayTracingService::onSync() {
        if (!ApplicationContext::Get().vulkanContext.rayTracingSupported) {
            return;
        }
        if (!needsRebuild) {
            return;
        }

        // Collect all mesh entities that have loaded mesh instances
        bool hasMeshes = false;
        for (auto &[entityId, meshComp]: ApplicationContext::Get().worldRepository.meshes) {
            if (meshComp.meshId.empty()) continue;
            auto *instance = ApplicationContext::Get().streamingRepository.streamMesh(meshComp.meshId, LevelOfDetail::LOD_0);
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

        // Create/update TLAS descriptor
        if (tlas != VK_NULL_HANDLE) {
            if (ApplicationContext::Get().coreDescriptorSets.tlasDescriptor == nullptr) {
                ApplicationContext::Get().coreDescriptorSets.tlasDescriptor = std::make_unique<DescriptorInstance>();
                ApplicationContext::Get().coreDescriptorSets.tlasDescriptor->addLayoutBinding(
                    DescriptorBinding::OfAccelerationStructure(
                        VK_SHADER_STAGE_RAYGEN_BIT_KHR, 0, tlas));
                ApplicationContext::Get().coreDescriptorSets.tlasDescriptor->create();
            } else {
                // Update existing descriptor
                std::vector<DescriptorBinding> bindings;
                bindings.push_back(
                    DescriptorBinding::OfAccelerationStructure(
                        VK_SHADER_STAGE_RAYGEN_BIT_KHR, 0, tlas));
                DescriptorInstance::Write(ApplicationContext::Get().coreDescriptorSets.tlasDescriptor->vkDescriptorSet, bindings);
            }
            accelerationStructureBuilt = true;
            needsRebuild = false;
        }
    }

    void RayTracingService::buildBLAS() {
        auto &vulkan = ApplicationContext::Get().vulkanContext;

        // Gather all geometry from all mesh entities into a single BLAS
        // We combine all triangles into one BLAS with multiple geometries
        std::vector<VkAccelerationStructureGeometryKHR> geometries;
        std::vector<VkAccelerationStructureBuildRangeInfoKHR> buildRangeInfos;
        std::vector<uint32_t> maxPrimitiveCounts;

        for (auto &[entityId, meshComp]: ApplicationContext::Get().worldRepository.meshes) {
            if (meshComp.meshId.empty()) continue;
            auto *instance = ApplicationContext::Get().streamingRepository.streamMesh(meshComp.meshId, LevelOfDetail::LOD_0);
            if (instance == nullptr || instance->dataBuffer == nullptr || instance->indexBuffer == nullptr) {
                continue;
            }

            VkDeviceAddress vertexAddress = getDeviceAddress(vulkan, instance->dataBuffer->vkBuffer);
            VkDeviceAddress indexAddress = getDeviceAddress(vulkan, instance->indexBuffer->vkBuffer);

            VkAccelerationStructureGeometryKHR geometry{};
            geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
            geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
            geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
            geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
            geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
            geometry.geometry.triangles.vertexData.deviceAddress = vertexAddress;
            geometry.geometry.triangles.vertexStride = sizeof(VertexData);
            geometry.geometry.triangles.maxVertex = static_cast<uint32_t>(instance->dataBuffer->dataSize / sizeof(VertexData)) - 1;
            geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
            geometry.geometry.triangles.indexData.deviceAddress = indexAddress;
            geometry.geometry.triangles.transformData.deviceAddress = 0; // identity

            geometries.push_back(geometry);

            uint32_t primitiveCount = instance->indexCount / 3;
            maxPrimitiveCounts.push_back(primitiveCount);

            VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
            rangeInfo.primitiveCount = primitiveCount;
            rangeInfo.primitiveOffset = 0;
            rangeInfo.firstVertex = 0;
            rangeInfo.transformOffset = 0;
            buildRangeInfos.push_back(rangeInfo);
        }

        if (geometries.empty()) return;

        // Get build sizes
        VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
        buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        buildInfo.geometryCount = static_cast<uint32_t>(geometries.size());
        buildInfo.pGeometries = geometries.data();

        VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
        vulkan.vkGetAccelerationStructureBuildSizesKHR(
            vulkan.device.device,
            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
            &buildInfo,
            maxPrimitiveCounts.data(),
            &sizeInfo);

        // Create BLAS buffer
        blasBuffer = ApplicationContext::Get().bufferService.createBuffer(
            sizeInfo.accelerationStructureSize,
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            true);

        // Create BLAS
        VkAccelerationStructureCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        createInfo.buffer = blasBuffer->vkBuffer;
        createInfo.size = sizeInfo.accelerationStructureSize;
        createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        VulkanUtils::CheckVKResult(
            vulkan.vkCreateAccelerationStructureKHR(vulkan.device.device, &createInfo, nullptr, &blas));

        // Create scratch buffer
        blasScratchBuffer = ApplicationContext::Get().bufferService.createBuffer(
            sizeInfo.buildScratchSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            true);

        buildInfo.dstAccelerationStructure = blas;
        buildInfo.scratchData.deviceAddress = getDeviceAddress(vulkan, blasScratchBuffer->vkBuffer);

        // Build BLAS
        VkCommandBuffer cmd = vulkan.beginSingleTimeCommands();
        const VkAccelerationStructureBuildRangeInfoKHR *pBuildRangeInfos = buildRangeInfos.data();
        vulkan.vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, &pBuildRangeInfos);
        vulkan.endSingleTimeCommands(cmd);
    }

    void RayTracingService::buildTLAS() {
        auto &vulkan = ApplicationContext::Get().vulkanContext;

        if (blas == VK_NULL_HANDLE) return;

        // Get BLAS device address
        VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
        addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        addressInfo.accelerationStructure = blas;
        VkDeviceAddress blasAddress = vulkan.vkGetAccelerationStructureDeviceAddressKHR(vulkan.device.device, &addressInfo);

        // Create one TLAS instance per mesh entity with its transform
        std::vector<VkAccelerationStructureInstanceKHR> instances;
        for (auto &[entityId, meshComp]: ApplicationContext::Get().worldRepository.meshes) {
            if (meshComp.meshId.empty()) continue;
            auto *meshInstance = ApplicationContext::Get().streamingRepository.streamMesh(meshComp.meshId, LevelOfDetail::LOD_0);
            if (meshInstance == nullptr || meshInstance->dataBuffer == nullptr) continue;

            // Get transform
            glm::mat4 model = glm::mat4(1.0f);
            if (ApplicationContext::Get().worldRepository.transforms.contains(entityId)) {
                model = ApplicationContext::Get().worldRepository.transforms[entityId].model;
            }

            // Convert glm::mat4 to VkTransformMatrixKHR (3x4 row-major)
            VkTransformMatrixKHR transform{};
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 4; col++) {
                    transform.matrix[row][col] = model[col][row]; // glm is column-major
                }
            }

            VkAccelerationStructureInstanceKHR instance{};
            instance.transform = transform;
            instance.instanceCustomIndex = 0;
            instance.mask = 0xFF;
            instance.instanceShaderBindingTableRecordOffset = 0;
            instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
            instance.accelerationStructureReference = blasAddress;

            instances.push_back(instance);
        }

        if (instances.empty()) return;

        // Upload instances
        instancesBuffer = ApplicationContext::Get().bufferService.createBuffer(
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

        tlasBuffer = ApplicationContext::Get().bufferService.createBuffer(
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

        tlasScratchBuffer = ApplicationContext::Get().bufferService.createBuffer(
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
        auto &vulkan = ApplicationContext::Get().vulkanContext;

        if (tlas != VK_NULL_HANDLE) {
            vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, tlas, nullptr);
            tlas = VK_NULL_HANDLE;
        }
        if (blas != VK_NULL_HANDLE) {
            vulkan.vkDestroyAccelerationStructureKHR(vulkan.device.device, blas, nullptr);
            blas = VK_NULL_HANDLE;
        }

        blasBuffer = nullptr;
        tlasBuffer = nullptr;
        instancesBuffer = nullptr;
        blasScratchBuffer = nullptr;
        tlasScratchBuffer = nullptr;
        accelerationStructureBuilt = false;
    }
} // Metal
