#ifndef FILESCONTEXT_H
#define FILESCONTEXT_H
#include <utility>
#include <vector>

#include "../../../common/util/files/FileEntry.h"

namespace Metal {
    struct FilesContext final {
        std::vector<FileEntry> entriesCurrentDir{};
        FileEntry rootDirectory;
        FileEntry *currentDirectory;
        std::unordered_map<std::string, FileEntry *> selected{};
        std::unordered_map<std::string, FileEntry *> toCut{};

        FilesContext(std::string rootPath, const std::vector<FileEntry> &files_current_dir)
            : entriesCurrentDir(files_current_dir),
              rootDirectory({"Root", std::move(rootPath), "", EntryType::DIRECTORY, ""}),
              currentDirectory(&rootDirectory) {
        }
    };
}
#endif //FILESCONTEXT_H
