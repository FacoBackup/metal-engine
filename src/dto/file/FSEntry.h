#ifndef FILEENTRY_H
#define FILEENTRY_H
#include <string>

#include "../../util/Util.h"
#include "EntryMetadata.h"

namespace Metal {
    struct FSEntry final : EntryMetadata {
        std::string absolutePath;
        std::string formattedDate;
        std::string formattedSize;
        bool isHovered = false;
        std::vector<FSEntry *> children{};
        FSEntry *parent = nullptr;

        FSEntry(FSEntry *parent, std::string absolute_path, std::string formatted_date)
            : absolutePath(std::move(absolute_path)),
              formattedDate(std::move(formatted_date)),
              parent(parent) {
        }
    };
}
#endif //FILEENTRY_H
