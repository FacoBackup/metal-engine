//
// Created by russi on 2/18/2026.
//

#include "VoxelService.h"

#include "SVOInstance.h"

namespace Metal {

    SVOInstance* VoxelService::create(const std::string &id) {
        // if (std::string fileName = ApplicationContext::Get().getAssetDirectory() + FORMAT_FILE_SVO(id);
        //   std::filesystem::exists(fileName)) {
        //     LOG_INFO("Streaming SVO " + fileName);
        //     auto data = SparseVoxelOctreeData();
        //     PARSE_TEMPLATE(data.load, fileName)
        //
        //     auto *instance = new SVOInstance(id);
        //     registerResource(instance);
        //     instance->voxelsBuffer = ApplicationContext::Get().bufferService.createBuffer(
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
} // Metal