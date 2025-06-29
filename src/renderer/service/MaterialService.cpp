#include "MaterialService.h"

#include "../../common/engine-definitions.h"

#include "../VulkanContext.h"
#include "../../common/FilesUtil.h"
#include "../data/MaterialInfo.h"

#include <cereal/archives/binary.hpp>
#include <fstream>

#include "../../MetalContextProvider.h"
#include "../../common/enum/LevelOfDetail.h"
#include "../../runtime/data/components/MeshComponent.h"
#include "../data/FrameBufferInstance.h"

namespace Metal {
    MaterialInfo *MaterialService::create(const std::string &id) {
        return stream(id);
    }

    MaterialInfo *MaterialService::stream(const std::string &id) const {
        auto pathToFile = ContextProvider::Get().getAssetDirectory() + FORMAT_FILE_MATERIAL(id);
        if (std::filesystem::exists(pathToFile)) {
            auto *data = new MaterialInfo;
            PARSE_TEMPLATE(data->load, pathToFile);
            return data;
        }
        return nullptr;
    }
} // Metal
