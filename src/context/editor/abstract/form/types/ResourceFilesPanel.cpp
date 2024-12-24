#include "ResourceFilesPanel.h"

#include "../../../../../common/interface/Icons.h"
#include "../../../../../dto/file/FileEntry.h"
#include "../../../../../service/files/FilesService.h"

namespace Metal {
    std::function<void()> ResourceFilesPanel::onAction() {
        return [this]() {
            callback(nullptr);
        };
    }

    std::string ResourceFilesPanel::getActionLabel() {
        return Icons::close + "Close";
    }

    void ResourceFilesPanel::openResource(FileEntry *root) {
        if (root->type == EntryType::DIRECTORY) {
            filesContext.setCurrentDirectory(root);
            FilesService::GetEntries(root);
            filesContext.selected.clear();
        } else {
            callback(root);
        }
    }
}
