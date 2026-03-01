#include "FilesRepository.h"

#include "../../common/interface/Icons.h"

namespace Metal {
    void FilesRepository::registerFields() {
        registerResourceSelection(materialId, "", "Material", EntryType::MATERIAL, false);
    }

    const char * FilesRepository::getIcon() {
        return Icons::file_open.c_str();
    }

    const char * FilesRepository::getTitle() {
        return "File Inspection";
    }
} // Metal
