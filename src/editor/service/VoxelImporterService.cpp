#include "VoxelImporterService.h"

#include <filesystem>
#include "common/serialization-definitions.h"

#include "../enum/engine-definitions.h"
#include "common/LoggerUtil.h"
#include <openvdb/openvdb.h>

#include <mutex>

#include "engine/dto/SparseVoxelOctreeData.h"

namespace fs = std::filesystem;

namespace Metal {

    std::string VoxelImporterService::importData(const std::string &targetDir,
                                             const std::string &pathToFile, const std::shared_ptr<ImportSettingsDTO> &settings, const std::stop_token &stopToken) {
        try {
            std::string fileName = fs::path(pathToFile).stem().string();
            std::string outPath = (fs::path(targetDir) / (fileName + ".svo")).string();

            convertToSVO(pathToFile, outPath, stopToken);

            LOG_INFO("Imported volume: " + fileName + " (" + outPath + ")");
            return outPath;
        } catch (std::exception &e) {
            LOG_ERROR(std::string("Volume import failed: ") + e.what());
            throw std::runtime_error("Volume import failed");
        }
    }

    std::vector<std::string> VoxelImporterService::getSupportedTypes() {
        return std::vector<std::string>{"vdb"};
    }

    size_t VoxelImporterService::convertToSVO(const std::string &sourcePath, const std::string &outPath, const std::stop_token &stopToken) const {
        static std::once_flag openvdbInitOnce;
        std::call_once(openvdbInitOnce, []() {
            openvdb::initialize();
        });

        int resolution = 12;
        // auto builder = SparseVoxelOctreeBuilder(BoundingBox{glm::vec3(-32, -32, -32), glm::vec3(32, 32, 32), glm::vec3(0, 0, 0)}, 32);
        //
        // try {
        //     openvdb::io::File file(sourcePath);
        //     file.open();
        //     openvdb::GridPtrVecPtr gridsPtr = file.getGrids();
        //     file.close();
        //     if (!gridsPtr) {
        //         throw std::runtime_error("No grids found in VDB file.");
        //     }
        //
        //     if (stopToken.stop_requested()) {
        //         return 0;
        //     }
        //
        //     for (const auto &gridPtr: *gridsPtr) {
        //         if (auto floatGrid = openvdb::gridPtrCast<openvdb::FloatGrid>(gridPtr)) {
        //             for (auto iter = floatGrid->beginValueOn(); iter; ++iter) {
        //                 if (stopToken.stop_requested()) {
        //                     return 0;
        //                 }
        //                 const openvdb::Coord xyz = iter.getCoord();
        //                 const openvdb::Vec3d worldPos = floatGrid->transform().indexToWorld(xyz);
        //                 const glm::vec3 volumePoint(worldPos.x(), worldPos.y(), worldPos.z());
        //
        //                 const glm::vec3 albedo(0.5f);
        //                 const glm::vec3 normal(0.5f);
        //                 const VoxelData data{albedo, normal, true};
        //
        //                 builder.insert(resolution, volumePoint, data);
        //             }
        //             break;
        //         }
        //     }
        // } catch (const std::exception &e) {
        //     throw std::runtime_error("VDB conversion failed: " + std::string(e.what()));
        // }

        // return serialize(builder, outPath);
        return 0;
    }

    void VoxelImporterService::FillStorage(SparseVoxelOctreeBuilder &builder, unsigned int &bufferIndex,
                                      unsigned int &materialBufferIndex,
                                      SparseVoxelOctreeData &voxels, OctreeNode *node) {
        if (node->isLeaf) {
            return;
        }

        voxels.data[node->dataIndex] = node->packVoxelData(bufferIndex);
        bool isParentOfLeaf = true;
        for (auto &child: node->children) {
            if (child != nullptr && !child->isLeaf) {
                PutData(bufferIndex, child);
                isParentOfLeaf = false;
            }
        }

        for (auto &child: node->children) {
            if (child != nullptr) {
                FillStorage(builder, bufferIndex, materialBufferIndex, voxels, child);
            }
        }

        if (isParentOfLeaf) {
            std::array<unsigned int, 2> &compressed = node->data;
            voxels.data[node->dataIndex] = node->packVoxelData(materialBufferIndex);
            voxels.data[materialBufferIndex + voxels.voxelBufferOffset] = compressed[0];
            materialBufferIndex++;
            voxels.data[materialBufferIndex + voxels.voxelBufferOffset] = compressed[1];
            materialBufferIndex++;
        }
    }

    void VoxelImporterService::PutData(unsigned int &bufferIndex, OctreeNode *node) {
        node->dataIndex = bufferIndex;
        bufferIndex++;
    }

    size_t VoxelImporterService::serialize(SparseVoxelOctreeBuilder &builder, const std::string &filePath) const {
        SparseVoxelOctreeData data{};
        data.data.resize(builder.getVoxelQuantity() + builder.getLeafVoxelQuantity() * 2);
        data.voxelBufferOffset = builder.getVoxelQuantity();

        unsigned int bufferIndex = 0;
        unsigned int materialBufferIndex = 0;
        PutData(bufferIndex, &builder.getRoot());
        FillStorage(builder, bufferIndex, materialBufferIndex, data, &builder.getRoot());

        DUMP_TEMPLATE(filePath, data)
        return fs::file_size(filePath);
    }

} // Metal
