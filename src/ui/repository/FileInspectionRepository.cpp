#include "FileInspectionRepository.h"

#include "../../common/enum/Icons.h"

namespace Metal {
    void FileInspectionRepository::registerFields() {
        registerResourceSelection(materialId, "", "Material", EntryType::MATERIAL, false);
    }

    const char * FileInspectionRepository::getIcon() {
        return Icons::file_open.c_str();
    }

    const char * FileInspectionRepository::getTitle() {
        return "File Inspection";
    }
} // Metal
