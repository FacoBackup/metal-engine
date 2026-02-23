#ifndef METAL_ENGINE_VOLUMEIMPORTERSERVICE_H
#define METAL_ENGINE_VOLUMEIMPORTERSERVICE_H

#include "../abstract/AbstractImporter.h"
#include "impl/SparseVoxelOctreeBuilder.h"
#include "impl/SparseVoxelOctreeData.h"

namespace Metal {
    struct SVOInstance;

    class VoxelImporterService final : public AbstractImporter {
    public:
        std::vector<std::string> getSupportedTypes() override;

        size_t convertToSVO(const std::string &sourcePath, const std::string &outPath,
                          const std::stop_token &stopToken) const;

        static void FillStorage(SparseVoxelOctreeBuilder &builder, unsigned int &bufferIndex,
                                unsigned int &materialBufferIndex,
                                SparseVoxelOctreeData &voxels, OctreeNode *node);

        static void PutData(unsigned int &bufferIndex, OctreeNode *node);

        size_t serialize(SparseVoxelOctreeBuilder &builder, const std::string &filePath) const;

        std::string importData(const std::string &targetDir, const std::string &pathToFile,
                               const std::shared_ptr<ImportSettingsDTO> &settings,
                               const std::stop_token &stopToken) override;
    };
} // Metal

#endif //METAL_ENGINE_VOLUMEIMPORTERSERVICE_H
