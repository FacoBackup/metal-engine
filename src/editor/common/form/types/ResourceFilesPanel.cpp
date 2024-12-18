#include "ResourceFilesPanel.h"

#include "../../../../common/interface/Icons.h"
#include "../../../../common/util/files/FileEntry.h"
#include "../../../service/FilesService.h"

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
