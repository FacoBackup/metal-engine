#include "MaterialService.h"

#include "../../enum/engine-definitions.h"

#include "../../context/vulkan/VulkanContext.h"
#include "../../util/FilesUtil.h"
#include "../../dto/buffers/MaterialInfo.h"

#include <cereal/archives/binary.hpp>
#include <fstream>

#include "../../context/ApplicationContext.h"
#include "../../enum/LevelOfDetail.h"
#include "../../repository/world/components/MeshComponent.h"
#include "../../service/framebuffer/FrameBufferInstance.h"

namespace Metal {
    MaterialInfo *MaterialService::create(const std::string &id) {
        return stream(id);
    }

    MaterialInfo *MaterialService::stream(const std::string &id) const {
        auto pathToFile = context.getAssetDirectory() + FORMAT_FILE_MATERIAL(id);
        if (std::filesystem::exists(pathToFile)) {
            auto *data = new MaterialInfo;
            PARSE_TEMPLATE(data->load, pathToFile);
            return data;
        }
        return nullptr;
    }
} // Metal
