#ifndef LEVELOFDETAIL_H
#define LEVELOFDETAIL_H
#include <string>
#define LOD_COUNT 4
namespace Metal {
    enum class EntryType;

    struct LevelOfDetail {
        static LevelOfDetail LOD_0;
        static LevelOfDetail LOD_1;
        static LevelOfDetail LOD_2;
        static LevelOfDetail LOD_3;

        const int level;
        const std::string suffix;

        static std::string GetFormattedName(const std::string &baseName, const LevelOfDetail &lod, EntryType entryType);
    };
}
#endif //LEVELOFDETAIL_H
