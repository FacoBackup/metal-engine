#ifndef FILEENTRY_H
#define FILEENTRY_H
#include <string>
#include <vector>

#include "common/ISerialize.h"

namespace Metal {
    struct FSEntry {
        std::string name;
        size_t size = 0;
        std::string absolutePath;
        std::string formattedDate;
        bool isDirectory = false;
        std::string formattedSize;
        std::string extension;
        bool isHovered = false;
        std::vector<std::shared_ptr<FSEntry>> children{};

        FSEntry(std::string absolute_path, std::string formatted_date)
            : absolutePath(std::move(absolute_path)),
              formattedDate(std::move(formatted_date)) {
        }

        std::string getAbsolutePath() const {
            return absolutePath;
        }
    };
}
#endif //FILEENTRY_H
