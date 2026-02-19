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

        explicit VoxelImporterService(ApplicationContext &context)
            : AbstractImporter(context) {
        }

        void convertToSVO(const std::string &sourcePath, const std::string &outPath,
                          const std::stop_token &stopToken) const;

        static void FillStorage(SparseVoxelOctreeBuilder &builder, unsigned int &bufferIndex,
                                unsigned int &materialBufferIndex,
                                SparseVoxelOctreeData &voxels, OctreeNode *node);

        static void PutData(unsigned int &bufferIndex, OctreeNode *node);

        void serialize(SparseVoxelOctreeBuilder &builder, const std::string &filePath) const;

        std::string importData(const std::string &targetDir, const std::string &pathToFile,
                               const std::stop_token &stopToken) override;
    };
} // Metal

#endif //METAL_ENGINE_VOLUMEIMPORTERSERVICE_H
