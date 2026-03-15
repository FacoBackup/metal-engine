#include "VoxelService.h"

#include "../resource/SVOInstance.h"

namespace Metal {

    SVOInstance* VoxelService::create(const std::string &id) {
        // if (std::string fileName = CTX.getAssetDirectory() + FORMAT_FILE_SVO(id);
        //   std::filesystem::exists(fileName)) {
        //     LOG_INFO("Streaming SVO " + fileName);
        //     auto data = SparseVoxelOctreeData();
        //     PARSE_TEMPLATE(data.load, fileName)
        //
        //     auto *instance = new SVOInstance(id);
        //     registerResource(instance);
        //     instance->voxelsBuffer = CTX.bufferService.createBuffer(
        //         data.data.size() * sizeof(unsigned int),
        //         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        //         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        //         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        //     instance->voxelsBuffer->update(data.data.data());
        //     instance->voxelBufferOffset = data.voxelBufferOffset;
        //
        //     return instance;
        //   }
         return nullptr;
    }

    void VoxelService::disposeResource(SVOInstance *resource) {
        if (resource->voxelsBuffer != nullptr) {
            dispose(resource->voxelsBuffer->getId());
        }
    }
} // Metal