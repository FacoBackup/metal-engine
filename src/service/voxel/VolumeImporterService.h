#ifndef METAL_ENGINE_VOLUMEIMPORTERSERVICE_H
#define METAL_ENGINE_VOLUMEIMPORTERSERVICE_H

#include "../abstract/AbstractImporter.h"

namespace Metal {
    class VolumeImporterService final : public AbstractImporter {
    public:
        std::vector<std::string> getSupportedTypes() override;

        explicit VolumeImporterService(ApplicationContext &context)
            : AbstractImporter(context) {
        }

        void convertToSVO(const std::string &sourcePath, const std::string &outPath,
                          const std::stop_token &stopToken) const;

        std::string importData(const std::string &targetDir, const std::string &pathToFile,
                        const std::stop_token &stopToken) override;

        void openVolume(const std::string &id) const;
    };
} // Metal

#endif //METAL_ENGINE_VOLUMEIMPORTERSERVICE_H
