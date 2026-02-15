#ifndef METAL_ENGINE_VOLUMEIMPORTERSERVICE_H
#define METAL_ENGINE_VOLUMEIMPORTERSERVICE_H

#include "../abstract/AbstractResourceService.h"

#include <string>

namespace Metal {
    class VolumeImporterService final : public AbstractResourceService {
    public:
        explicit VolumeImporterService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        [[nodiscard]] bool isCompatible(const std::string &pathToFile) const;

        std::string importVolume(const std::string &targetDir, const std::string &pathToFile) const;

        void openVolume(const std::string &id) const;
    };
} // Metal

#endif //METAL_ENGINE_VOLUMEIMPORTERSERVICE_H
