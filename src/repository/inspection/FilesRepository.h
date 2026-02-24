#ifndef FILEINSPECTIONREPOSITORY_H
#define FILEINSPECTIONREPOSITORY_H
#include <unordered_map>

#include "../../common/inspection/Inspectable.h"

namespace Metal {
    struct FSEntry;
    struct ImportSettingsDTO;

    struct FilesRepository final : Inspectable {
        std::string materialId;

        std::vector<std::string> pendingImports;
        std::unordered_map<std::string, std::shared_ptr<ImportSettingsDTO> > importSettingsMap;
        std::string selectedFileForSettings;
        FSEntry *targetImportDirectory = nullptr;

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override;
    };
} // Metal

#endif //FILEINSPECTIONREPOSITORY_H
