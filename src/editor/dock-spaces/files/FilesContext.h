#ifndef FILESCONTEXT_H
#define FILESCONTEXT_H

#include <string>
#include <unordered_map>
#include "../../enum/EntryType.h"


namespace Metal {
    struct FSEntry;

    struct FilesContext final {
        FSEntry *currentDirectory = nullptr;
        std::unordered_map<std::string, FSEntry *> selected{};
        std::unordered_map<std::string, FSEntry *> toCut{};
        std::string pathToCurrentDirectory;
        EntryType::EntryType filterType = EntryType::NONE;

        explicit FilesContext(FSEntry *currentDirectory)
            : currentDirectory(currentDirectory) {
        }

        void setCurrentDirectory(FSEntry *entry);
    };
}
#endif //FILESCONTEXT_H
