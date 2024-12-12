#ifndef FILEENTRY_H
#define FILEENTRY_H
#include <string>

#include "../../../common/util/Util.h"
#include "EntryType.h"

namespace Metal {
    struct FileEntry final {
        std::string name;
        std::string absolutePath;
        std::string formattedDate;
        EntryType type;
        std::string formattedSize;
        bool isHovered = false;
        const std::string id = Util::uuidV4();
        std::vector<FileEntry> children{};
        FileEntry *parent = nullptr;

        FileEntry(FileEntry *parent, std::string name, std::string absolute_path, std::string formatted_date,
                  EntryType type, std::string formatted_size)
            : parent(parent), name(std::move(name)),
              absolutePath(std::move(absolute_path)),
              formattedDate(std::move(formatted_date)),
              type(type),
              formattedSize(std::move(formatted_size)) {
        }
    };
}
#endif //FILEENTRY_H
