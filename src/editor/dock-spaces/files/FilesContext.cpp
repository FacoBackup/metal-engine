#include "FilesContext.h"
#include "../../dto/FSEntry.h"
#include "../../service/FilesService.h"
#include <memory>

namespace Metal {
    void FilesContext::setCurrentDirectory(std::shared_ptr<FSEntry> entry) {
        currentDirectory = entry;

        if (filesService == nullptr || filesService->getRoot() == nullptr) {
            pathToCurrentDirectory = "";
            return;
        }

        auto rootPath = filesService->getRoot()->absolutePath;
        if (currentDirectory->absolutePath.size() >= rootPath.size()) {
            pathToCurrentDirectory = currentDirectory->absolutePath.substr(
                rootPath.size(), currentDirectory->absolutePath.size());
        } else {
            pathToCurrentDirectory = "";
        }
    }
}
