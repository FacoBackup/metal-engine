#include "LevelOfDetail.h"

#include "../../common/util/files/EntryType.h"
#include "../engine-definitions.h"

namespace Metal {
    LevelOfDetail LevelOfDetail::LOD_0{1, "LOD-0"};
    LevelOfDetail LevelOfDetail::LOD_1{2, "LOD-1"};
    LevelOfDetail LevelOfDetail::LOD_2{4, "LOD-2"};
    LevelOfDetail LevelOfDetail::LOD_3{8, "LOD-3"};

    std::string LevelOfDetail::GetFormattedName(const std::string &baseName, const LevelOfDetail &lod,
                                                const EntryType entryType) {
        switch (entryType) {
            case EntryType::MESH:
                return baseName + FILE_NAME_SEPARATOR + lod.suffix + FILE_MESH + METAL_FILE_EXTENSION;
            case EntryType::TEXTURE:
                return baseName + FILE_NAME_SEPARATOR + lod.suffix + FILE_TEXTURE + METAL_FILE_EXTENSION;

            case EntryType::MATERIAL:
                return baseName + FILE_NAME_SEPARATOR + lod.suffix + FILE_MATERIAL + METAL_FILE_EXTENSION;
            default:
                return "";
        }
    }
}
