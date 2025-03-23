#ifndef FILEITEM_H
#define FILEITEM_H
#include <string>

namespace Metal {
    struct FileItem final {
        std::string name;
        bool isDirectory;
    };
}
#endif //FILEITEM_H
