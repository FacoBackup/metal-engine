#include "ResourceFilesPanel.h"

#include "../../../../common/Icons.h"
#include "../../../dto/FSEntry.h"
#include "editor/service/FilesService.h"

namespace Metal {
    std::function<void()> ResourceFilesPanel::onAction() {
        return [this]() {
            callback(nullptr);
        };
    }

    std::string ResourceFilesPanel::getActionLabel() {
        return Icons::close + "Close";
    }

    void ResourceFilesPanel::openResource(FSEntry *root) {
        if (root->type == EntryType::DIRECTORY) {
            filesContext.setCurrentDirectory(root);
            filesService->GetEntries(root);
            filesContext.selected.clear();
        } else {
            callback(root);
        }
    }
}
