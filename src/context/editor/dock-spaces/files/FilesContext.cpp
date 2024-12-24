#include "FilesContext.h"

#include "../../../../dto/file/FileEntry.h"
#include <memory>

namespace Metal {
    void FilesContext::setCurrentDirectory(FileEntry *entry) {
        currentDirectory = entry;

        auto *root = currentDirectory;
        while (root->parent != nullptr) {
            root = root->parent;
        }
        pathToCurrentDirectory = currentDirectory->absolutePath.substr(
            root->absolutePath.size(), currentDirectory->absolutePath.size());
    }
}
