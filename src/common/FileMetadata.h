#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include <string>
#include "Util.h"
#include "enum/EntryType.h"
#include "serialization-definitions.h"

namespace Metal {
    struct FileMetadata {
    protected:
        std::string id = Util::uuidV4();

    public:
        std::string name;
        EntryType::EntryType type = EntryType::NONE;

        std::string getId() {
            return id;
        }

        SAVE_TEMPLATE(name, id, type)
    };
}
#endif //FILEMETADATA_H
