#include "VoxelService.h"
#include "../resource/SVOInstance.h"
#include "../../common/LoggerUtil.h"
#include "../../common/serialization-definitions.h"
#include "../repository/EngineRepository.h"
#include "BufferService.h"
#include "../dto/SparseVoxelOctreeData.h"
#include "../../ApplicationEventContext.h"
#include "../dto/ResourceDisposalPayload.h"
#include "common/FileExtensions.h"

namespace Metal {
    void VoxelService::onInitialize() {
        eventListener([this](const Event &event) {
            auto payload = std::dynamic_pointer_cast<ResourceDisposalPayload>(event.payload);
            if (payload) {
                dispose(payload->resourceId);
            }
        }, "RESOURCE_DISPOSAL");
    }

    void VoxelService::load(const std::string &svoPath) {
        if (std::filesystem::exists(svoPath)) {
            LOG_INFO("Streaming SVO " + svoPath);
            auto data = SparseVoxelOctreeData();
            PARSE_TEMPLATE(data, svoPath)

            auto *instance = createResourceInstance(svoPath);
            instance->voxelsBuffer = bufferService->createBuffer(
                instance->getId() + "_voxels",
                data.data.size() * sizeof(unsigned int),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            instance->voxelsBuffer->update(data.data.data());
            instance->voxelBufferOffset = data.voxelBufferOffset;
        }
    }

    bool VoxelService::isCompatible(const std::string &absolutePath) {
        return absolutePath.ends_with(Metal::FileExtensions::svo->extension);
    }

    void VoxelService::disposeResource(SVOInstance *resource) {
    }
} // Metal
