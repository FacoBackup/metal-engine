#include "SVOService.h"

#include <cereal/archives/binary.hpp>
#include <vulkan/vulkan_core.h>

#include "SVOInstance.h"
#include "../../enum/engine-definitions.h"
#include "../../enum/LevelOfDetail.h"
#include "../../context/ApplicationContext.h"
#include "../../util/serialization-definitions.h"
#include "impl/SparseVoxelOctreeData.h"

namespace Metal {
    SVOInstance *SVOService::create(const std::string &id) {
        if (std::string fileName = context.getAssetDirectory() + FORMAT_FILE_SVO(id);
            std::filesystem::exists(fileName)) {
            std::cout << "Streaming SVO " << fileName << std::endl;
            auto data = SparseVoxelOctreeData();
            PARSE_TEMPLATE(data.load, fileName)

            auto *instance = new SVOInstance(id);
            registerResource(instance);
            instance->voxelsBuffer = context.bufferService.createBuffer(
                data.data.size() * sizeof(uint32_t),
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            instance->voxelsBuffer->update(data.data.data());
            instance->voxelBufferOffset = data.voxelBufferOffset;

            return instance;
        }
        return nullptr;
    }
} // Metal
