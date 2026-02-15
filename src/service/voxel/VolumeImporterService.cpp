#include "VolumeImporterService.h"

#include <algorithm>
#include <filesystem>

#include <cereal/archives/binary.hpp>

#include "../voxel/impl/SnapshotWorldTile.h"
#include "../../context/ApplicationContext.h"
#include "../../dto/file/EntryMetadata.h"
#include "../../enum/EntryType.h"
#include "../../enum/engine-definitions.h"
#include "../../service/log/LogService.h"
#include <openvdb/openvdb.h>
#include <openvdb/io/File.h>
#include <glm/glm.hpp>

#include <mutex>

namespace fs = std::filesystem;

namespace Metal {

    std::string VolumeImporterService::importData(const std::string &targetDir,
                                             const std::string &pathToFile, const std::stop_token &stopToken) {
        try {
            auto metadata = EntryMetadata{};
            metadata.type = EntryType::VOLUME;
            metadata.name = fs::path(pathToFile).filename().string();
            metadata.name = metadata.name.substr(0, metadata.name.find_last_of('.'));


            std::string outPath = context.getAssetDirectory() + FORMAT_FILE_VOLUME(metadata.getId());
            convertToSVO(pathToFile, outPath, stopToken);

            DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(metadata.getId()), metadata)
            LOG_INFO(context, "Imported volume: " + metadata.name + " (" + metadata.getId() + ")");
            return metadata.getId();
        } catch (std::exception &e) {
            LOG_ERROR(context, std::string("Volume import failed: ") + e.what());
            throw std::runtime_error("Volume import failed");
        }
    }

    std::vector<std::string> VolumeImporterService::getSupportedTypes() {
        return std::vector<std::string>{"vdb"};
    }

    void VolumeImporterService::convertToSVO(const std::string &sourcePath, const std::string &outPath, const std::stop_token &stopToken) const {
        static std::once_flag openvdbInitOnce;
        std::call_once(openvdbInitOnce, []() {
            openvdb::initialize();
        });

        auto *targetTile = context.worldGridRepository.getTile(glm::vec3(0, 0, 0));
        int resolution = 12;
        auto builder = SparseVoxelOctreeBuilder(
            new SnapshotWorldTile(targetTile->x, targetTile->z, targetTile->id, targetTile->boundingBox));

        try {
            openvdb::io::File file(sourcePath);
            file.open();
            openvdb::GridPtrVecPtr gridsPtr = file.getGrids();
            file.close();
            if (!gridsPtr) {
                throw std::runtime_error("No grids found in VDB file.");
            }

            if (stopToken.stop_requested()) {
                return;
            }

            for (const auto &gridPtr: *gridsPtr) {
                if (auto floatGrid = openvdb::gridPtrCast<openvdb::FloatGrid>(gridPtr)) {
                    for (auto iter = floatGrid->beginValueOn(); iter; ++iter) {
                        if (stopToken.stop_requested()) {
                            return;
                        }
                        const openvdb::Coord xyz = iter.getCoord();
                        const openvdb::Vec3d worldPos = floatGrid->transform().indexToWorld(xyz);
                        const glm::vec3 volumePoint(worldPos.x(), worldPos.y(), worldPos.z());

                        const glm::vec3 albedo(0.5f);
                        const glm::vec3 normal(0.5f);
                        const VoxelData data{albedo, normal, true};

                        builder.insert(resolution, volumePoint, data);
                    }
                    break;
                }
            }
        } catch (const std::exception &e) {
            throw std::runtime_error("VDB conversion failed: " + std::string(e.what()));
        }

        context.voxelizationService.serialize(builder, outPath);
    }

    void VolumeImporterService::openVolume(const std::string &id) const {
        // TODO: Replace with actual volume/voxel/VDB streaming and visualization.
        LOG_INFO(context, "Open Volume requested: " + id);
    }
} // Metal
