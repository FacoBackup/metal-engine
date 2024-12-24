#ifndef FILESCONTEXT_H
#define FILESCONTEXT_H

#include <string>
#include <unordered_map>
#include "../../../../enum/EntryType.h"


namespace Metal {
    struct FileEntry;

    struct FilesContext final {
        FileEntry *currentDirectory = nullptr;
        std::unordered_map<std::string, FileEntry *> selected{};
        std::unordered_map<std::string, FileEntry *> toCut{};
        std::string pathToCurrentDirectory;
        EntryType::EntryType filterType = EntryType::NONE;

        explicit FilesContext(FileEntry *currentDirectory)
            : currentDirectory(currentDirectory) {
        }

        void setCurrentDirectory(FileEntry *entry);
    };
}
#endif //FILESCONTEXT_H
