#include <engine/service/TLASService.h>
#include <core/VulkanContext.h>
#include <engine/service/BufferService.h>
#include <engine/service/BLASService.h>
#include <engine/repository/WorldRepository.h>
#include <engine/service/DescriptorSetService.h>
#include <engine/service/PipelineService.h>
#include <engine/resource/BufferInstance.h>
#include <engine/dto/PrimitiveComponent.h>
#include <engine/dto/TransformComponent.h>
#include <common/VulkanUtils.h>
#include <common/LoggerUtil.h>
#include <editor/enum/engine-definitions.h>
#include <engine/dto/DescriptorInstance.h>
#include <engine/dto/DescriptorBinding.h>
#include <ApplicationEventContext.h>

namespace Metal {
    void TLASService::onInitialize() {
        eventListener([this](const Event &) {
            needsRebuild = true;
        }, "TransformComponent");

        eventListener([this](const Event &) {
            needsRebuild = true;
        }, "PrimitiveComponent", "BVHNeedsUpdate");
    }

    void TLASService::onSync() {
        if (!vulkanContext->rayTracingSupported) {
            return;
        }
        if (worldRepository->dirtyEntities.empty() && !needsRebuild) {
            return;
        }

        buildTLAS();

        if (tlas != VK_NULL_HANDLE) {
            LOG_INFO("TLAS Updated");
            ApplicationEventContext::dispatch("BVHUpdated");
        } else {
            LOG_WARN("TLAS build failed or resulted in NULL handle");
        }
        needsRebuild = false;
        worldRepository->dirtyEntities.clear();
    }

    VkDeviceAddress TLASService::getDeviceAddress(VkBuffer buffer) const {
        VkBufferDeviceAddressInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = buffer;
        return vulkanContext->vkGetBufferDeviceAddressKHR(vulkanContext->device.device, &info);
    }

    void TLASService::updateDescriptorSets(VkAccelerationStructureKHR asHandle) const {
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

    void TLASService::dispose() {
        if (!vulkanContext || !vulkanContext->rayTracingSupported) return;

        if (vulkanContext->device.device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(vulkanContext->device.device);
        }

        if (tlas != VK_NULL_HANDLE) {
            vulkanContext->vkDestroyAccelerationStructureKHR(vulkanContext->device.device, tlas, nullptr);
            tlas = VK_NULL_HANDLE;
        }

        if (tlasBuffer) {
            bufferService->dispose(tlasBuffer->getId());
            tlasBuffer = nullptr;
        }
        if (instancesBuffer) {
            bufferService->dispose(instancesBuffer->getId());
            instancesBuffer = nullptr;
        }
        if (tlasScratchBuffer) {
            bufferService->dispose(tlasScratchBuffer->getId());
            tlasScratchBuffer = nullptr;
        }
    }

    void TLASService::buildTLAS() {
        if (!vulkanContext->rayTracingSupported) return;

        dispose();

        std::vector<VkAccelerationStructureInstanceKHR> instances;
        auto view = worldRepository->registry.view<PrimitiveComponent, TransformComponent>();

        uint32_t currentInstanceIndex = 0;
        for (auto entity: view) {
            if (currentInstanceIndex >= MAX_MESH_INSTANCES) {
                LOG_ERROR("Max mesh instances reached for ray tracing: " + std::to_string(MAX_MESH_INSTANCES));
                break;
            }
            if (worldRepository->hiddenEntities.contains(entity)) continue;
            auto &meshComp = view.get<PrimitiveComponent>(entity);
            if (meshComp.meshId.empty()) continue;

            VkDeviceAddress blasAddress = blasService->getBLASAddress(meshComp.meshId);
            if (blasAddress == 0) continue;

            glm::mat4 model = view.get<TransformComponent>(entity).model;

            VkTransformMatrixKHR transform{};
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 4; col++) {
                    transform.matrix[row][col] = model[col][row];
                }
            }

            meshComp.renderIndex = currentInstanceIndex;

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

        if (instances.empty()) {
            updateDescriptorSets(VK_NULL_HANDLE);
            return;
        }

        instancesBuffer = bufferService->createBuffer(
            "tlas_instances_" + Util::uuidV4(),
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
            "tlas_buffer_" + Util::uuidV4(),
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
            "tlas_scratch_" + Util::uuidV4(),
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

        updateDescriptorSets(tlas);
    }
}
