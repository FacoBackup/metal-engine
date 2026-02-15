#include "VolumeImporterService.h"

#include <algorithm>
#include <filesystem>

#include <cereal/archives/binary.hpp>

#include "../../context/ApplicationContext.h"
#include "../../dto/file/EntryMetadata.h"
#include "../../enum/EntryType.h"
#include "../../enum/engine-definitions.h"
#include "../../service/log/LogService.h"
#include "../../util/FilesUtil.h"

namespace fs = std::filesystem;

namespace Metal {
    static std::string toLower(std::string s) {
        std::ranges::transform(s, s.begin(), [](const unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    bool VolumeImporterService::isCompatible(const std::string &pathToFile) const {
        auto ext = toLower(fs::path(pathToFile).extension().string());
        return ext == ".vdb" || ext == ".pcd" || ext == ".ply" || ext == ".xyz" || ext == ".las" || ext == ".laz" ||
               ext == ".e57";
    }

    std::string VolumeImporterService::importVolume(const std::string &targetDir,
                                                    const std::string &pathToFile) const {
        try {
            auto metadata = EntryMetadata{};
            metadata.type = EntryType::VOLUME;
            metadata.name = fs::path(pathToFile).filename().string();
            metadata.name = metadata.name.substr(0, metadata.name.find_last_of('.'));

            DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(metadata.getId()), metadata)

            const auto outPath = context.getAssetDirectory() + FORMAT_FILE_VOLUME(metadata.getId());
            fs::copy_file(pathToFile, outPath, fs::copy_options::overwrite_existing);

            LOG_INFO(context, "Imported volume: " + metadata.name + " (" + metadata.getId() + ")");
            return metadata.getId();
        } catch (std::exception &e) {
            LOG_ERROR(context, std::string("Volume import failed: ") + e.what());
            return "";
        }
    }

    void VolumeImporterService::openVolume(const std::string &id) const {
        // TODO: Replace with actual volume/voxel/VDB streaming and visualization.
        LOG_INFO(context, "Open Volume requested: " + id);
    }
} // Metal
