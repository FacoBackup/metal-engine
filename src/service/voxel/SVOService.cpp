#include "SVOService.h"

#include <cereal/archives/binary.hpp>
#include <vulkan/vulkan_core.h>

#include "SVOInstance.h"
#include "../../enum/engine-definitions.h"
#include "../../context/ApplicationContext.h"
#include "../../util/serialization-definitions.h"
#include "impl/SparseVoxelOctreeData.h"

namespace Metal {
    SVOInstance *SVOService::create(const std::string &id) {
        if (std::filesystem::exists(FORMAT_FILE_SVO(id))) {
            auto data = SparseVoxelOctreeData();
            PARSE_TEMPLATE(data.load, FORMAT_FILE_SVO(id))

            auto *instance = new SVOInstance(id);
            registerResource(instance);
            instance->buffer = context.bufferService.createBuffer(sizeof(data),
                                                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            return instance;
        }
        return nullptr;
    }
} // Metal
