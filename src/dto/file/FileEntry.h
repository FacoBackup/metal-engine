#ifndef FILEENTRY_H
#define FILEENTRY_H
#include <string>

#include "../../util/Util.h"
#include "FileMetadata.h"

namespace Metal {
    struct FileEntry final : FileMetadata {
        std::string absolutePath;
        std::string formattedDate;
        std::string formattedSize;
        bool isHovered = false;
        std::vector<FileEntry *> children{};
        FileEntry *parent = nullptr;

        FileEntry(FileEntry *parent, std::string absolute_path, std::string formatted_date,
                  std::string formatted_size)
            : absolutePath(std::move(absolute_path)),
              formattedDate(std::move(formatted_date)),
              formattedSize(std::move(formatted_size)),
              parent(parent) {
        }
    };
}
#endif //FILEENTRY_H
