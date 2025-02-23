#include "MaterialService.h"

#include "../../enum/engine-definitions.h"
#include "MaterialData.h"

#include "../../context/vulkan/VulkanContext.h"
#include "../../util/FilesUtil.h"

#include <cereal/archives/binary.hpp>
#include <fstream>

#include "MaterialInstance.h"
#include "../../context/ApplicationContext.h"
#include "../../enum/LevelOfDetail.h"
#include "../../repository/world/components/MeshComponent.h"
#include "../texture/TextureInstance.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferInstance.h"

namespace Metal {
    MaterialInstance *MaterialService::create(const std::string &id) {
        // TODO - Rewrite material system
        return nullptr;
    }

    MaterialData *MaterialService::stream(const std::string &id) const {
        auto pathToFile = context.getAssetDirectory() + FORMAT_FILE_MATERIAL(id);
        if (std::filesystem::exists(pathToFile)) {
            MaterialData *data = new MaterialData;
            PARSE_TEMPLATE(data->load, pathToFile);
            return data;
        }
        return nullptr;
    }
} // Metal
