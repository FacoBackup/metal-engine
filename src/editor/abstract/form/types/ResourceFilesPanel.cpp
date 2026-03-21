#include "ResourceFilesPanel.h"

#include "../../../../common/Icons.h"
#include "../../../dto/FSEntry.h"
#include "editor/service/FilesService.h"

namespace Metal {
    void ResourceFilesPanel::openResource(std::shared_ptr<FSEntry> root) {
        if (root->isDirectory) {
            filesContext.setCurrentDirectory(root);
            filesService->GetEntries(root);
            filesContext.selected.clear();
        } else {
            callback(root);
        }
    }
}
