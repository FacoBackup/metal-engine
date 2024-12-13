#ifndef FILESCONTEXT_H
#define FILESCONTEXT_H

#include <memory>
#include <string>
#include <unordered_map>

namespace Metal {
    struct FileEntry;

    struct FilesContext final {
        FileEntry *currentDirectory = nullptr;
        std::unordered_map<std::string, FileEntry *> selected{};
        std::unordered_map<std::string, FileEntry *> toCut{};
        std::string pathToCurrentDirectory;

        explicit FilesContext(FileEntry *currentDirectory)
            : currentDirectory(currentDirectory) {
        }

        void setCurrentDirectory(FileEntry *entry);
    };
}
#endif //FILESCONTEXT_H
