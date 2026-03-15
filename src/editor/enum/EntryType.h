#ifndef FILETYPE_H
#define FILETYPE_H

#include <string>
#include <vector>
#include "../../common/Icons.h"

namespace Metal::EntryType {
    enum EntryType {
        SCENE,
        MESH,
        TEXTURE,
        VOLUME,
        DIRECTORY,
        NONE
    };

    struct EntryTypeDTO {
        EntryType type;
        std::string name;
        std::string icon;
    };

    static const std::vector<EntryTypeDTO> ENTRIES = {
            {SCENE,     "Scene",     Icons::image},
            {MESH,      "Mesh",      Icons::view_in_ar},
            {TEXTURE,   "Texture",   Icons::texture},
            {VOLUME,    "Volume",    Icons::view_agenda},
            {DIRECTORY, "Directory", Icons::folder},
            {NONE,      "None",      Icons::close}
    };

    static const EntryTypeDTO& GetDTO(EntryType type) {
        for (const auto& dto : ENTRIES) {
            if (dto.type == type) {
                return dto;
            }
        }
        return ENTRIES.back(); // Return NONE
    }
}
#endif
