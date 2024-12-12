#ifndef FILESCONTEXT_H
#define FILESCONTEXT_H
#include <utility>
#include <vector>

#include "../../../common/util/files/FileEntry.h"

namespace Metal {
    struct FilesContext final {
        FileEntry rootDirectory;
        FileEntry *currentDirectory;
        std::unordered_map<std::string, FileEntry *> selected{};
        std::unordered_map<std::string, FileEntry *> toCut{};
        std::string pathToCurrentDirectory;

        explicit FilesContext(std::string rootPath)
            : rootDirectory({nullptr, "Root", std::move(rootPath), "", EntryType::DIRECTORY, ""}),
              currentDirectory(&rootDirectory) {
        }

        void setCurrentDirectory(FileEntry *entry) {
            if (entry == nullptr)
                return;
            currentDirectory = entry;
            pathToCurrentDirectory = currentDirectory->absolutePath.substr(
                rootDirectory.absolutePath.size(), currentDirectory->absolutePath.size());
        }
    };
}
#endif //FILESCONTEXT_H
