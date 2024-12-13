#ifndef FILETYPE_H
#define FILETYPE_H

#include <nlohmann/json.hpp>

namespace Metal {
    enum class EntryType {
        MESH,
        TEXTURE,
        MATERIAL,
        DIRECTORY,
        NONE
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(EntryType, {
                                 {EntryType::MESH, "MESH"},
                                 {EntryType::TEXTURE, "TEXTURE"},
                                 {EntryType::MATERIAL, "MATERIAL"},
                                 {EntryType::DIRECTORY, "DIRECTORY"}
                                 })
}
#endif
